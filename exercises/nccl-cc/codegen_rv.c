/*
 * codegen_rv.c - RISC-V code generator for nccl-cc
 *
 * Lesson 73: return <number>
 * Lesson 74: unary -, !, ~
 * Lesson 75: binary +, -, *, /, % (stack-based evaluation)
 * Lesson 76: comparison ==, !=, <, <=, >, >= and logic &&, ||
 * Lesson 77: local variables (lw/sw via s0 offset)
 * Lesson 78: assignment, expression statements, blocks
 */
#include "nccl_cc.h"

static int label_count = 0;
static const char *cur_fn_name;
static int cur_fn_stack;

/* break/continue loop context stack */
#define MAX_LOOP_DEPTH 32
static int loop_break_labels[MAX_LOOP_DEPTH];
static int loop_cont_labels[MAX_LOOP_DEPTH];
static int loop_depth = 0;

/* string literal storage */
#define MAX_STRINGS 256
static struct {
    int id;
    char *val;
} strings[MAX_STRINGS];
static int nstrings = 0;

/* NOTE (teaching simplification): expression pushes move sp by 4 bytes,
 * so sp is not kept 16-byte aligned as the RISC-V psABI requires.
 * Our own runtime does not care; linking against a real libc would. */
static void push(void) {
    printf("    addi sp, sp, -4\n");
    printf("    sw a0, 0(sp)\n");
}

static void pop(const char *reg) {
    printf("    lw %s, 0(sp)\n", reg);
    printf("    addi sp, sp, 4\n");
}

/* Materialize `s0 - offset` into reg. addi/lw/sw immediates are
 * 12-bit signed (max 2047), so big frames go through t6 - a scratch
 * register nothing else in this backend uses. */
static void addr_of_local(const char *reg, int offset) {
    if (offset <= 2047) {
        printf("    addi %s, s0, -%d\n", reg, offset);
    } else {
        printf("    li t6, %d\n", offset);
        printf("    sub %s, s0, t6\n", reg);
    }
}

/* Store reg (width-aware) into the local at s0-offset without
 * clobbering anything but t6. */
static void store_local(const char *reg, int offset, int size) {
    const char *op = (size == 1) ? "sb" : "sw";
    if (offset <= 2047) {
        printf("    %s %s, -%d(s0)\n", op, reg, offset);
    } else {
        addr_of_local("t6", offset);
        printf("    %s %s, 0(t6)\n", op, reg);
    }
}

/* forward declarations */
static void gen_expr(Node *node);
static void gen_stmt(Node *node);

/* Load a value of type ty from the address in a0 (width follows the
 * type: char→1 byte sign-extended via lb, int/pointer→4 bytes on RV32).
 * Arrays and structs are used by address — no load. */
static void load_val(Type *ty) {
    if (ty && (ty->kind == TY_ARRAY || ty->kind == TY_STRUCT)) return;
    if (ty && ty->size == 1)
        printf("    lb a0, 0(a0)\n");
    else
        printf("    lw a0, 0(a0)\n");
}

/* Store the value in a0 (of type ty) to the address in t0. */
static void store_val(Type *ty) {
    if (ty && ty->size == 1)
        printf("    sb a0, 0(t0)\n");
    else
        printf("    sw a0, 0(t0)\n");
}

/* L77: generate address of lvalue into a0 */
static void gen_addr(Node *node) {
    if (node->kind == ND_VAR) {
        if (node->var->is_global)
            printf("    la a0, %s\n", node->var->name);
        else
            addr_of_local("a0", node->var->offset);
        return;
    }
    /* L88: dereference as lvalue */
    if (node->kind == ND_DEREF) {
        gen_expr(node->lhs);
        return;
    }
    /* L91: struct member as lvalue: base address + member offset */
    if (node->kind == ND_MEMBER) {
        gen_addr(node->lhs);
        if (node->member && node->member->offset) printf("    addi a0, a0, %d\n", node->member->offset);
        return;
    }
    fprintf(stderr, "codegen: not an lvalue (kind=%d)\n", node->kind);
    exit(1);
}

static void gen_expr(Node *node) {
    if (!node) return;

    if (node->kind == ND_NUM) {
        printf("    li a0, %d\n", node->val);
        return;
    }

    /* L77/L86/L87: variable load (width from the annotated type) */
    if (node->kind == ND_VAR) {
        gen_addr(node);
        load_val(node->ty ? node->ty : (node->var ? node->var->ty : NULL));
        return;
    }

    /* L86: string literal */
    if (node->kind == ND_STR) {
        printf("    la a0, .L_str_%d\n", node->str_id);
        /* register string for .rodata output */
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
        printf("    beqz a0, .L_tern_f_%d\n", lbl);
        gen_expr(node->cond_true);
        printf("    j .L_tern_end_%d\n", lbl);
        printf(".L_tern_f_%d:\n", lbl);
        gen_expr(node->cond_false);
        printf(".L_tern_end_%d:\n", lbl);
        return;
    }

    /* comma operator: evaluate both, return right */
    if (node->kind == ND_COMMA) {
        gen_expr(node->lhs);
        gen_expr(node->rhs);
        return;
    }

    /* L91: struct member access: shared address path + typed load */
    if (node->kind == ND_MEMBER) {
        gen_addr(node);
        load_val(node->ty);
        return;
    }

    /* L78/L86: assignment (store width follows the lvalue's type) */
    if (node->kind == ND_ASSIGN) {
        gen_addr(node->lhs);
        push();
        gen_expr(node->rhs);
        pop("t0");
        store_val(node->lhs->ty ? node->lhs->ty : (node->lhs->var ? node->lhs->var->ty : NULL));
        return;
    }

    /* L77: variable declaration with initializer */
    if (node->kind == ND_VAR_DECL) {
        if (node->lhs) {
            gen_expr(node->lhs);
            store_local("a0", node->var->offset, (node->var && node->var->ty) ? node->var->ty->size : 4);
        }
        return;
    }

    /* L88: address-of */
    if (node->kind == ND_ADDR) {
        gen_addr(node->lhs);
        return;
    }

    /* L88: dereference (load width = the pointed-to type) */
    if (node->kind == ND_DEREF) {
        gen_expr(node->lhs);
        load_val(node->ty);
        return;
    }

    /* type cast: (char)x sign-extends the low byte, matching lb semantics */
    if (node->kind == ND_CAST) {
        gen_expr(node->lhs);
        if (node->ty && node->ty->kind == TY_CHAR) {
            printf("    slli a0, a0, 24\n");
            printf("    srai a0, a0, 24\n");
        }
        return;
    }

    /* L82: function call */
    if (node->kind == ND_CALL) {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr(arg);
            push();
            nargs++;
        }
        /* pop args into a0-a7 (reverse order) */
        static const char *areg[] = {"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7"};
        for (int i = nargs - 1; i >= 0; i--) pop(areg[i]);
        printf("    call %s\n", node->name);
        return;
    }

    /* unary operators (Lesson 74) */
    if (node->kind == ND_NEG) {
        gen_expr(node->lhs);
        printf("    neg a0, a0\n");
        return;
    }
    if (node->kind == ND_NOT) {
        gen_expr(node->lhs);
        printf("    seqz a0, a0\n");
        return;
    }
    if (node->kind == ND_BITNOT) {
        gen_expr(node->lhs);
        printf("    not a0, a0\n");
        return;
    }

    /* short-circuit logic (Lesson 76) */
    if (node->kind == ND_AND) {
        int lbl = label_count++;
        gen_expr(node->lhs);
        printf("    beqz a0, .L_false_%d\n", lbl);
        gen_expr(node->rhs);
        printf("    snez a0, a0\n");
        printf("    j .L_end_%d\n", lbl);
        printf(".L_false_%d:\n", lbl);
        printf("    li a0, 0\n");
        printf(".L_end_%d:\n", lbl);
        return;
    }
    if (node->kind == ND_OR) {
        int lbl = label_count++;
        gen_expr(node->lhs);
        printf("    bnez a0, .L_true_%d\n", lbl);
        gen_expr(node->rhs);
        printf("    snez a0, a0\n");
        printf("    j .L_end_%d\n", lbl);
        printf(".L_true_%d:\n", lbl);
        printf("    li a0, 1\n");
        printf(".L_end_%d:\n", lbl);
        return;
    }

    /* binary operators (Lesson 75-76): stack-based evaluation */
    gen_expr(node->rhs);
    push();
    gen_expr(node->lhs);
    pop("t0");

    switch (node->kind) {
        case ND_ADD:
            printf("    add a0, a0, t0\n");
            break;
        case ND_SUB:
            printf("    sub a0, a0, t0\n");
            break;
        case ND_MUL:
            printf("    mul a0, a0, t0\n");
            break;
        case ND_DIV:
            printf("    div a0, a0, t0\n");
            break;
        case ND_MOD:
            printf("    rem a0, a0, t0\n");
            break;
        case ND_BITAND:
            printf("    and a0, a0, t0\n");
            break;
        case ND_BITOR:
            printf("    or a0, a0, t0\n");
            break;
        case ND_BITXOR:
            printf("    xor a0, a0, t0\n");
            break;
        case ND_SHL:
            printf("    sll a0, a0, t0\n");
            break;
        case ND_SHR:
            printf("    sra a0, a0, t0\n");
            break;
        case ND_EQ:
            printf("    sub a0, a0, t0\n");
            printf("    seqz a0, a0\n");
            break;
        case ND_NE:
            printf("    sub a0, a0, t0\n");
            printf("    snez a0, a0\n");
            break;
        case ND_LT:
            printf("    slt a0, a0, t0\n");
            break;
        case ND_LE:
            printf("    slt a0, t0, a0\n");
            printf("    xori a0, a0, 1\n");
            break;
        case ND_GT:
            printf("    slt a0, t0, a0\n");
            break;
        case ND_GE:
            printf("    slt a0, a0, t0\n");
            printf("    xori a0, a0, 1\n");
            break;
        default:
            fprintf(stderr, "codegen: unexpected binary op %d\n", node->kind);
            exit(1);
    }
}

static void gen_stmt(Node *node) {
    if (!node) return;

    switch (node->kind) {
        case ND_RETURN:
            if (node->lhs) gen_expr(node->lhs);
            printf("    j .L_return_%s\n", cur_fn_name);
            return;

        /* L79: if/else */
        case ND_IF: {
            int lbl = label_count++;
            gen_expr(node->cond);
            printf("    beqz a0, .L_else_%d\n", lbl);
            gen_stmt(node->then);
            printf("    j .L_endif_%d\n", lbl);
            printf(".L_else_%d:\n", lbl);
            if (node->els) gen_stmt(node->els);
            printf(".L_endif_%d:\n", lbl);
            return;
        }

        /* L80: while — with break/continue support */
        case ND_WHILE: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = lbl;
            loop_depth++;
            printf(".L_while_%d:\n", lbl);
            printf(".L_cont_%d:\n", lbl);
            gen_expr(node->cond);
            printf("    beqz a0, .L_wend_%d\n", lbl);
            gen_stmt(node->body);
            printf("    j .L_while_%d\n", lbl);
            printf(".L_wend_%d:\n", lbl);
            printf(".L_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        /* L80: for — with break/continue support */
        case ND_FOR: {
            int lbl = label_count++;
            int cont_lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = cont_lbl;
            loop_depth++;
            if (node->init) gen_stmt(node->init);
            printf(".L_for_%d:\n", lbl);
            if (node->cond) {
                gen_expr(node->cond);
                printf("    beqz a0, .L_fend_%d\n", lbl);
            }
            gen_stmt(node->body);
            printf(".L_cont_%d:\n", cont_lbl);
            if (node->inc) gen_expr(node->inc);
            printf("    j .L_for_%d\n", lbl);
            printf(".L_fend_%d:\n", lbl);
            printf(".L_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        /* L81: do-while — with break/continue support */
        case ND_DOWHILE: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = lbl;
            loop_depth++;
            printf(".L_do_%d:\n", lbl);
            printf(".L_cont_%d:\n", lbl);
            gen_stmt(node->body);
            gen_expr(node->cond);
            printf("    bnez a0, .L_do_%d\n", lbl);
            printf(".L_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        /* switch/case codegen */
        case ND_SWITCH: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl; /* break exits switch */
            loop_depth++;
            gen_expr(node->cond);
            /* generate comparison jumps for each case */
            int case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++) {
                if (c->kind == ND_CASE) {
                    printf("    li t0, %d\n", c->case_val);
                    printf("    beq a0, t0, .L_case_%d_%d\n", lbl, case_num);
                }
            }
            /* jump to default or end */
            int def_num = -1;
            case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++)
                if (c->kind == ND_DEFAULT) def_num = case_num;
            if (def_num >= 0)
                printf("    j .L_case_%d_%d\n", lbl, def_num);
            else
                printf("    j .L_brk_%d\n", lbl);
            /* generate case bodies (each body is a statement LIST) */
            case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++) {
                printf(".L_case_%d_%d:\n", lbl, case_num);
                for (Node *s = c->body; s; s = s->next) gen_stmt(s);
            }
            printf(".L_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        case ND_BREAK:
            if (loop_depth > 0) printf("    j .L_brk_%d\n", loop_break_labels[loop_depth - 1]);
            return;

        case ND_CONTINUE:
            if (loop_depth > 0) printf("    j .L_cont_%d\n", loop_cont_labels[loop_depth - 1]);
            return;

        case ND_BLOCK:
            for (Node *s = node->body; s; s = s->next) gen_stmt(s);
            return;

        case ND_EXPR_STMT:
            gen_expr(node->lhs);
            return;

        case ND_VAR_DECL:
            gen_expr(node);
            return;

        default:
            fprintf(stderr, "codegen: unexpected statement kind %d\n", node->kind);
            exit(1);
    }
}

static void gen_function(Node *fn) {
    if (fn->kind != ND_FUNC) return;

    cur_fn_name = fn->name;
    cur_fn_stack = fn->stack_size + 16; /* +16 for ra/s0 save area */

    printf("\n.globl %s\n", fn->name);
    printf("%s:\n", fn->name);

    /* prologue — frame layout (high → low):
     *   [s0-1   .. s0-stack_size]  locals (addressed as -off(s0))
     *   [sp+8   .. sp+15]          padding
     *   [sp+4]                     saved s0
     *   [sp+0]                     saved ra
     * ra/s0 are saved at the BOTTOM so they can never collide with the
     * locals that grow down from s0 (saving at the top overlapped the
     * first local slot and clobbered the return address). */
    if (cur_fn_stack <= 2047) {
        printf("    addi sp, sp, -%d\n", cur_fn_stack);
        printf("    sw ra, 0(sp)\n");
        printf("    sw s0, 4(sp)\n");
        printf("    addi s0, sp, %d\n", cur_fn_stack);
    } else {
        printf("    li t6, %d\n", cur_fn_stack);
        printf("    sub sp, sp, t6\n");
        printf("    sw ra, 0(sp)\n");
        printf("    sw s0, 4(sp)\n");
        printf("    add s0, sp, t6\n");
    }

    /* L83: store parameters from a0-a7 to stack (width-aware) */
    int i = 0;
    for (Node *p = fn->params; p && i < 8; p = p->next, i++) {
        if (!p->var) continue;
        char reg[4];
        snprintf(reg, sizeof(reg), "a%d", i);
        store_local(reg, p->var->offset, (p->var->ty) ? p->var->ty->size : 4);
    }

    /* generate body */
    gen_stmt(fn->body);

    /* epilogue */
    printf(".L_return_%s:\n", fn->name);
    printf("    lw ra, 0(sp)\n");
    printf("    lw s0, 4(sp)\n");
    if (cur_fn_stack <= 2047) {
        printf("    addi sp, sp, %d\n", cur_fn_stack);
    } else {
        printf("    li t6, %d\n", cur_fn_stack);
        printf("    add sp, sp, t6\n");
    }
    printf("    ret\n");
}

void codegen_rv(Node *prog) {
    /* L85: .data section for global variables */
    int has_data = 0;
    for (Var *v = globals; v; v = v->next) {
        if (!has_data) {
            printf(".data\n");
            has_data = 1;
        }
        printf(".globl %s\n", v->name);
        printf("%s:\n", v->name);
        if (v->ty && v->ty->kind == TY_ARRAY)
            printf("    .zero %d\n", v->ty->size);
        else
            printf("    .word %d\n", v->init_val);
    }

    /* .text section for functions */
    printf("\n.text\n");
    for (Node *fn = prog->body; fn; fn = fn->next) gen_function(fn);

    /* .rodata section for string literals */
    if (nstrings > 0) {
        printf("\n.section .rodata\n");
        for (int i = 0; i < nstrings; i++) {
            printf(".L_str_%d:\n", strings[i].id);
            printf("    .string ");
            emit_escaped_string(strings[i].val);
            printf("\n");
        }
    }

    /* mark the stack non-executable (silences modern ld warnings) */
    printf("\n.section .note.GNU-stack,\"\",@progbits\n");
}
