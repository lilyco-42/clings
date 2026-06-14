/*
 * codegen_a64.c - AArch64 (ARM64) code generator for nccl-cc
 *
 * Lesson 96: the third register architecture, second LP64 target.
 *
 * What makes AArch64 different from our other backends:
 *
 *   1. LP64 data model (like x86-64): int is 4 bytes, pointers are 8.
 *      Width selection is type-driven: int values live in w registers,
 *      pointers in x registers. Writing a w register implicitly zeroes
 *      the upper 32 bits of the corresponding x register.
 *
 *   2. HARDWARE-ENFORCED stack alignment: unlike x86-64 (where 16-byte
 *      alignment is only an ABI convention checked at call sites), the
 *      AArch64 core faults on any sp-relative access while sp is not
 *      16-byte aligned. Expression pushes therefore move sp in 16-byte
 *      steps - wasteful, but always correct, and it keeps the
 *      teaching-friendly "one value, one push" stack model.
 *
 *   3. Pointer arithmetic mixes widths: the scaled index is computed
 *      in 32 bits, so the pointer add/sub uses the extended-register
 *      form (`add x0, x0, w1, sxtw`) to sign-extend NEGATIVE offsets.
 *      This mirrors the movslq dance in codegen_x86.c.
 *
 *   4. Comparisons read the condition flags via cset - compare this
 *      with RISC-V where slt/seqz compose the result without flags.
 *
 * Output targets ELF (Linux). Runs under qemu-aarch64 with our
 * runtime_a64.s, or natively on any AArch64 Linux box.
 */
#include "nccl_cc.h"

static int label_count = 0;
static const char *cur_fn_name;

/* break/continue loop context stack */
#define MAX_LOOP_DEPTH 32
static int loop_break_labels[MAX_LOOP_DEPTH];
static int loop_cont_labels[MAX_LOOP_DEPTH];
static int loop_depth = 0;

/* string literal storage */
#define MAX_STRINGS 256
static struct { int id; char *val; } strings[MAX_STRINGS];
static int nstrings = 0;

/* Load a 32-bit constant into a register. mov handles the encodable
 * range (movz/movn aliases cover [-65536, 65535]); anything wider is
 * built from a movz/movk pair. `reg` is a w register name. */
static void load_imm(const char *reg, int val)
{
    if (val >= -65536 && val <= 65535) {
        printf("    mov %s, #%d\n", reg, val);
    } else {
        printf("    mov %s, #%d\n", reg, val & 0xffff);
        printf("    movk %s, #%d, lsl #16\n", reg, ((unsigned)val >> 16) & 0xffff);
    }
}

/* Materialize `x29 - offset` into register xreg (an x register name).
 * sub's immediate field is 12 bits; larger frames go through x9. */
static void addr_of_local(const char *xreg, int offset)
{
    if (offset <= 4095) {
        printf("    sub %s, x29, #%d\n", xreg, offset);
    } else {
        load_imm("w9", offset);
        printf("    sub %s, x29, x9\n", xreg);
    }
}

/* The expression stack: sp must stay 16-byte aligned at ALL times on
 * AArch64 (hardware check), so each push claims a full 16-byte slot.
 * We always push/pop the full x register - for int values the upper
 * 32 bits are already zero (w-register writes clear them). */
static void push(void)
{
    printf("    str x0, [sp, #-16]!\n");
}

static void pop(const char *xreg)
{
    printf("    ldr %s, [sp], #16\n", xreg);
}

/* forward declarations */
static void gen_expr(Node *node);
static void gen_stmt(Node *node);

static int is_ptr_node(Node *n)
{
    return n && n->ty && is_pointer_like(n->ty);
}

/* Load a value of type ty from the address in x0.
 * char -> ldrsb (sign-extend), int -> 4 bytes, pointer -> 8 bytes.
 * Arrays and structs are used by address - no load. */
static void load_val(Type *ty)
{
    if (ty && (ty->kind == TY_ARRAY || ty->kind == TY_STRUCT))
        return;
    if (ty && ty->size == 1)
        printf("    ldrsb w0, [x0]\n");
    else if (ty && ty->size == 8)
        printf("    ldr x0, [x0]\n");
    else
        printf("    ldr w0, [x0]\n");
}

/* Store the value in x0/w0 (of type ty) to the address in x1. */
static void store_val(Type *ty)
{
    if (ty && ty->size == 1)
        printf("    strb w0, [x1]\n");
    else if (ty && ty->size == 8)
        printf("    str x0, [x1]\n");
    else
        printf("    str w0, [x1]\n");
}

/* Generate the address of an lvalue into x0. */
static void gen_addr(Node *node)
{
    if (node->kind == ND_VAR) {
        if (node->var->is_global) {
            printf("    adrp x0, %s\n", node->var->name);
            printf("    add x0, x0, :lo12:%s\n", node->var->name);
        } else {
            addr_of_local("x0", node->var->offset);
        }
        return;
    }
    if (node->kind == ND_DEREF) {
        gen_expr(node->lhs);
        return;
    }
    if (node->kind == ND_MEMBER) {
        gen_addr(node->lhs);
        if (node->member && node->member->offset)
            printf("    add x0, x0, #%d\n", node->member->offset);
        return;
    }
    fprintf(stderr, "a64 codegen: not an lvalue (kind=%d)\n", node->kind);
    exit(1);
}

static void gen_expr(Node *node)
{
    if (!node) return;

    if (node->kind == ND_NUM) {
        load_imm("w0", node->val);
        return;
    }

    /* variable load (width from the annotated type) */
    if (node->kind == ND_VAR) {
        gen_addr(node);
        load_val(node->ty ? node->ty : (node->var ? node->var->ty : NULL));
        return;
    }

    /* string literal: page-relative addressing (ELF) */
    if (node->kind == ND_STR) {
        printf("    adrp x0, .L64_str_%d\n", node->str_id);
        printf("    add x0, x0, :lo12:.L64_str_%d\n", node->str_id);
        if (nstrings < MAX_STRINGS) {
            strings[nstrings].id = node->str_id;
            strings[nstrings].val = node->str_val;
            nstrings++;
        }
        return;
    }

    /* ternary ? : */
    if (node->kind == ND_COND) {
        int lbl = label_count++;
        gen_expr(node->cond);
        printf("    cbz w0, .L64_tern_f_%d\n", lbl);
        gen_expr(node->cond_true);
        printf("    b .L64_tern_end_%d\n", lbl);
        printf(".L64_tern_f_%d:\n", lbl);
        gen_expr(node->cond_false);
        printf(".L64_tern_end_%d:\n", lbl);
        return;
    }

    /* comma operator: evaluate both, keep the right value */
    if (node->kind == ND_COMMA) {
        gen_expr(node->lhs);
        gen_expr(node->rhs);
        return;
    }

    /* struct member access: shared address path + typed load */
    if (node->kind == ND_MEMBER) {
        gen_addr(node);
        load_val(node->ty);
        return;
    }

    /* assignment (store width follows the lvalue's type) */
    if (node->kind == ND_ASSIGN) {
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        pop("x1");
        store_val(node->lhs->ty ? node->lhs->ty
                                : (node->lhs->var ? node->lhs->var->ty : NULL));
        return;
    }

    /* variable declaration with initializer */
    if (node->kind == ND_VAR_DECL) {
        if (node->lhs) {
            gen_expr(node->lhs);
            addr_of_local("x9", node->var->offset);
            if (node->var && node->var->ty && node->var->ty->size == 1)
                printf("    strb w0, [x9]\n");
            else if (node->var && node->var->ty && node->var->ty->size == 8)
                printf("    str x0, [x9]\n");
            else
                printf("    str w0, [x9]\n");
        }
        return;
    }

    /* address-of */
    if (node->kind == ND_ADDR) {
        gen_addr(node->lhs);
        return;
    }

    /* dereference (load width = the pointed-to type) */
    if (node->kind == ND_DEREF) {
        gen_expr(node->lhs);
        load_val(node->ty);
        return;
    }

    /* type cast: (char)x sign-extends the low byte (sxtb = ldrsb
     * semantics); int/pointer casts need no code in this subset */
    if (node->kind == ND_CAST) {
        gen_expr(node->lhs);
        if (node->ty && node->ty->kind == TY_CHAR)
            printf("    sxtb w0, w0\n");
        return;
    }

    /* function call: AAPCS64 passes the first 8 args in x0-x7.
     * Args are pushed left-to-right, then popped right-to-left so
     * nested calls in argument position cannot clobber each other.
     * sp stays 16-byte aligned throughout (16-byte pushes), so the
     * alignment requirement at the bl is satisfied by construction. */
    if (node->kind == ND_CALL) {
        if (node->nargs > 8) {
            fprintf(stderr,
                    "a64 codegen: more than 8 arguments in call to '%s' "
                    "(stack argument passing is not implemented)\n",
                    node->name);
            exit(1);
        }
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr(arg);
            push();
            nargs++;
        }
        static const char *areg[] = {"x0","x1","x2","x3","x4","x5","x6","x7"};
        for (int i = nargs - 1; i >= 0; i--)
            pop(areg[i]);
        printf("    bl %s\n", node->name);
        return;
    }

    /* unary operators */
    if (node->kind == ND_NEG) {
        gen_expr(node->lhs);
        printf("    neg w0, w0\n");
        return;
    }
    if (node->kind == ND_NOT) {
        gen_expr(node->lhs);
        printf("    cmp w0, #0\n");
        printf("    cset w0, eq\n");
        return;
    }
    if (node->kind == ND_BITNOT) {
        gen_expr(node->lhs);
        printf("    mvn w0, w0\n");
        return;
    }

    /* short-circuit logic */
    if (node->kind == ND_AND) {
        int lbl = label_count++;
        gen_expr(node->lhs);
        printf("    cbz w0, .L64_false_%d\n", lbl);
        gen_expr(node->rhs);
        printf("    cmp w0, #0\n");
        printf("    cset w0, ne\n");
        printf("    b .L64_end_%d\n", lbl);
        printf(".L64_false_%d:\n", lbl);
        printf("    mov w0, #0\n");
        printf(".L64_end_%d:\n", lbl);
        return;
    }
    if (node->kind == ND_OR) {
        int lbl = label_count++;
        gen_expr(node->lhs);
        printf("    cbnz w0, .L64_true_%d\n", lbl);
        gen_expr(node->rhs);
        printf("    cmp w0, #0\n");
        printf("    cset w0, ne\n");
        printf("    b .L64_end_%d\n", lbl);
        printf(".L64_true_%d:\n", lbl);
        printf("    mov w0, #1\n");
        printf(".L64_end_%d:\n", lbl);
        return;
    }

    /* binary operators: stack-based evaluation.
     * rhs first, push; lhs second; pop rhs into x1.
     * Result convention: lhs in x0/w0, rhs in x1/w1. */
    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    pop("x1");

    int l_ptr = is_ptr_node(node->lhs);
    int r_ptr = is_ptr_node(node->rhs);

    switch (node->kind) {
    case ND_ADD:
        if (l_ptr || r_ptr) {
            /* ptr + scaled-int: sign-extend the 32-bit index so a
             * NEGATIVE offset doesn't become a 4GB-positive one
             * (the parser canonicalizes int+ptr to ptr+int) */
            printf("    add x0, x0, w1, sxtw\n");
        } else {
            printf("    add w0, w0, w1\n");
        }
        break;
    case ND_SUB:
        if (l_ptr && r_ptr) {
            /* ptr - ptr: 64-bit difference (parser divides by size) */
            printf("    sub x0, x0, x1\n");
        } else if (l_ptr) {
            printf("    sub x0, x0, w1, sxtw\n");
        } else {
            printf("    sub w0, w0, w1\n");
        }
        break;
    case ND_MUL: printf("    mul w0, w0, w1\n"); break;
    case ND_DIV: printf("    sdiv w0, w0, w1\n"); break;
    case ND_MOD:
        /* w9 = w0 / w1; w0 = w0 - w9 * w1 */
        printf("    sdiv w9, w0, w1\n");
        printf("    msub w0, w9, w1, w0\n");
        break;
    case ND_BITAND: printf("    and w0, w0, w1\n"); break;
    case ND_BITOR:  printf("    orr w0, w0, w1\n"); break;
    case ND_BITXOR: printf("    eor w0, w0, w1\n"); break;
    case ND_SHL: printf("    lsl w0, w0, w1\n"); break;
    case ND_SHR: printf("    asr w0, w0, w1\n"); break;
    case ND_EQ:
    case ND_NE:
    case ND_LT:
    case ND_LE:
    case ND_GT:
    case ND_GE: {
        /* pointer comparisons must use the full 64-bit width */
        if (l_ptr || r_ptr)
            printf("    cmp x0, x1\n");
        else
            printf("    cmp w0, w1\n");
        const char *cond =
            node->kind == ND_EQ ? "eq" :
            node->kind == ND_NE ? "ne" :
            node->kind == ND_LT ? "lt" :
            node->kind == ND_LE ? "le" :
            node->kind == ND_GT ? "gt" : "ge";
        printf("    cset w0, %s\n", cond);
        break;
    }
    default:
        fprintf(stderr, "a64 codegen: unexpected binary op %d\n", node->kind);
        exit(1);
    }
}

static void gen_stmt(Node *node)
{
    if (!node) return;

    switch (node->kind) {
    case ND_RETURN:
        if (node->lhs) gen_expr(node->lhs);
        printf("    b .L64_return_%s\n", cur_fn_name);
        return;

    case ND_IF: {
        int lbl = label_count++;
        gen_expr(node->cond);
        printf("    cbz w0, .L64_else_%d\n", lbl);
        gen_stmt(node->then);
        printf("    b .L64_endif_%d\n", lbl);
        printf(".L64_else_%d:\n", lbl);
        if (node->els) gen_stmt(node->els);
        printf(".L64_endif_%d:\n", lbl);
        return;
    }

    case ND_WHILE: {
        int lbl = label_count++;
        loop_break_labels[loop_depth] = lbl;
        loop_cont_labels[loop_depth] = lbl;
        loop_depth++;
        printf(".L64_while_%d:\n", lbl);
        printf(".L64_cont_%d:\n", lbl);
        gen_expr(node->cond);
        printf("    cbz w0, .L64_wend_%d\n", lbl);
        gen_stmt(node->body);
        printf("    b .L64_while_%d\n", lbl);
        printf(".L64_wend_%d:\n", lbl);
        printf(".L64_brk_%d:\n", lbl);
        loop_depth--;
        return;
    }

    case ND_FOR: {
        int lbl = label_count++;
        int cont_lbl = label_count++;
        loop_break_labels[loop_depth] = lbl;
        loop_cont_labels[loop_depth] = cont_lbl;
        loop_depth++;
        if (node->init) gen_stmt(node->init);
        printf(".L64_for_%d:\n", lbl);
        if (node->cond) {
            gen_expr(node->cond);
            printf("    cbz w0, .L64_fend_%d\n", lbl);
        }
        gen_stmt(node->body);
        printf(".L64_cont_%d:\n", cont_lbl);
        if (node->inc) gen_expr(node->inc);
        printf("    b .L64_for_%d\n", lbl);
        printf(".L64_fend_%d:\n", lbl);
        printf(".L64_brk_%d:\n", lbl);
        loop_depth--;
        return;
    }

    case ND_DOWHILE: {
        int lbl = label_count++;
        loop_break_labels[loop_depth] = lbl;
        loop_cont_labels[loop_depth] = lbl;
        loop_depth++;
        printf(".L64_do_%d:\n", lbl);
        printf(".L64_cont_%d:\n", lbl);
        gen_stmt(node->body);
        gen_expr(node->cond);
        printf("    cbnz w0, .L64_do_%d\n", lbl);
        printf(".L64_brk_%d:\n", lbl);
        loop_depth--;
        return;
    }

    case ND_SWITCH: {
        int lbl = label_count++;
        loop_break_labels[loop_depth] = lbl; /* break exits switch */
        loop_depth++;
        gen_expr(node->cond);
        int case_num = 0;
        for (Node *c = node->cases; c; c = c->next, case_num++) {
            if (c->kind == ND_CASE) {
                load_imm("w9", c->case_val);
                printf("    cmp w0, w9\n");
                printf("    b.eq .L64_case_%d_%d\n", lbl, case_num);
            }
        }
        int def_num = -1;
        case_num = 0;
        for (Node *c = node->cases; c; c = c->next, case_num++)
            if (c->kind == ND_DEFAULT) def_num = case_num;
        if (def_num >= 0)
            printf("    b .L64_case_%d_%d\n", lbl, def_num);
        else
            printf("    b .L64_brk_%d\n", lbl);
        /* generate case bodies (each body is a statement LIST) */
        case_num = 0;
        for (Node *c = node->cases; c; c = c->next, case_num++) {
            printf(".L64_case_%d_%d:\n", lbl, case_num);
            for (Node *s = c->body; s; s = s->next)
                gen_stmt(s);
        }
        printf(".L64_brk_%d:\n", lbl);
        loop_depth--;
        return;
    }

    case ND_BREAK:
        if (loop_depth > 0)
            printf("    b .L64_brk_%d\n", loop_break_labels[loop_depth - 1]);
        return;

    case ND_CONTINUE:
        if (loop_depth > 0)
            printf("    b .L64_cont_%d\n", loop_cont_labels[loop_depth - 1]);
        return;

    case ND_BLOCK:
        for (Node *s = node->body; s; s = s->next)
            gen_stmt(s);
        return;

    case ND_EXPR_STMT:
        gen_expr(node->lhs);
        return;

    case ND_VAR_DECL:
        gen_expr(node);
        return;

    default:
        fprintf(stderr, "a64 codegen: unexpected statement kind %d\n", node->kind);
        exit(1);
    }
}

static void gen_function(Node *fn)
{
    if (fn->kind != ND_FUNC) return;

    cur_fn_name = fn->name;
    int frame = align_to(fn->stack_size, 16);

    printf("\n.globl %s\n", fn->name);
    printf("%s:\n", fn->name);

    /* prologue - frame layout (high -> low):
     *   [x29+8]                  saved x30 (lr)
     *   [x29+0]                  saved x29 (caller's fp)   <- x29
     *   [x29-1 .. x29-frame]     locals (addressed as x29 - offset)
     *   [sp ...]                 16-byte expression-stack pushes
     * stp with pre-index keeps sp 16-aligned at every step. */
    printf("    stp x29, x30, [sp, #-16]!\n");
    printf("    mov x29, sp\n");
    if (frame > 0) {
        if (frame <= 4095) {
            printf("    sub sp, sp, #%d\n", frame);
        } else {
            load_imm("w9", frame);
            printf("    sub sp, sp, x9\n");
        }
    }

    /* spill parameters from x0-x7 to their stack slots (width-aware) */
    int i = 0;
    for (Node *p = fn->params; p && i < 8; p = p->next, i++) {
        if (!p->var) continue;
        addr_of_local("x9", p->var->offset);
        if (p->var->ty && p->var->ty->size == 1)
            printf("    strb w%d, [x9]\n", i);
        else if (p->var->ty && p->var->ty->size == 8)
            printf("    str x%d, [x9]\n", i);
        else
            printf("    str w%d, [x9]\n", i);
    }

    gen_stmt(fn->body);

    /* epilogue: collapse the frame, restore fp/lr, return */
    printf(".L64_return_%s:\n", fn->name);
    printf("    mov sp, x29\n");
    printf("    ldp x29, x30, [sp], #16\n");
    printf("    ret\n");
}

void codegen_a64(Node *prog)
{
    /* .data section for global variables */
    int has_data = 0;
    for (Var *v = globals; v; v = v->next) {
        if (!has_data) { printf(".data\n"); has_data = 1; }
        printf(".globl %s\n", v->name);
        printf("%s:\n", v->name);
        if (v->ty && v->ty->kind == TY_ARRAY)
            printf("    .zero %d\n", v->ty->size);
        else
            printf("    .word %d\n", v->init_val);
    }

    /* .text section for functions */
    printf("\n.text\n");
    for (Node *fn = prog->body; fn; fn = fn->next)
        gen_function(fn);

    /* .rodata section for string literals */
    if (nstrings > 0) {
        printf("\n.section .rodata\n");
        for (int i = 0; i < nstrings; i++) {
            printf(".L64_str_%d:\n", strings[i].id);
            printf("    .string ");
            emit_escaped_string(strings[i].val);
            printf("\n");
        }
    }

    /* mark the stack non-executable (silences modern ld warnings) */
    printf("\n.section .note.GNU-stack,\"\",@progbits\n");
}
