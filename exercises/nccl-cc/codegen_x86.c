/*
 * codegen_x86.c - x86-64 code generator for nccl-cc
 *
 * Third backend: generates x86-64 assembly (AT&T syntax, System V ABI).
 * Cross-platform: works on both Linux and macOS (Intel).
 * Link with: cc output.s -o output (uses libc, no -nostdlib needed).
 *
 * Key differences from RISC-V:
 *   a0       →  %rax (return value / accumulator)
 *   push/pop →  native push/pop instructions
 *   s0(fp)   →  %rbp (frame pointer)
 *   sp       →  %rsp (stack pointer)
 *   call     →  call (same!)
 *   ret      →  ret (same!)
 *   params   →  %rdi, %rsi, %rdx, %rcx, %r8, %r9
 */
#include "nccl_cc.h"

/* Platform-specific assembly syntax (Mach-O vs ELF).
 * Selected at RUNTIME so one binary can cross-emit for either OS
 * (-target=x86-mac / -target=x86-linux); the default follows the host.
 *   macOS:  symbols get '_' prefix, local labels start with 'L' (no dot),
 *           strings live in __TEXT,__cstring
 *   Linux:  no prefix, '.L' local labels, .section .rodata */
#ifdef __APPLE__
int x86_macos = 1;
#else
int x86_macos = 0;
#endif

static const char *sym_prefix;     /* "_" or ""  */
static const char *lp;             /* local label prefix: "L" or ".L" */
static const char *rodata_section; /* string literal section */
static const char *str_directive;  /* .asciz / .string */

static int label_count = 0;
static const char *cur_fn_name;
static int cur_fn_stack;

/* Current stack depth in 8-byte push units. SysV requires %rsp to be
 * 16-byte aligned AT every call instruction; the frame base is aligned,
 * so a call only needs padding when `depth` is odd (chibicc's trick). */
static int depth = 0;

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
} x86_strings[MAX_STRINGS];
static int x86_nstrings = 0;

static void push(void) {
    printf("    pushq %%rax\n");
    depth++;
}

static void pop(const char *reg) {
    printf("    popq %s\n", reg);
    depth--;
}

/* forward declarations */
static void gen_expr_x86(Node *node);
static void gen_stmt_x86(Node *node);

/* Load a value of type ty from the address in %rax.
 * Width comes from the TYPE, not from guessing: char→1 byte
 * (sign-extended), int→4 bytes, pointer→8 bytes on LP64.
 * Arrays and structs are used by address, so no load happens. */
static void load(Type *ty) {
    if (ty && (ty->kind == TY_ARRAY || ty->kind == TY_STRUCT)) return; /* address IS the value */
    int sz = ty ? ty->size : 4;
    if (sz == 1)
        printf("    movsbl (%%rax), %%eax\n");
    else if (sz == 8)
        printf("    movq (%%rax), %%rax\n");
    else
        printf("    movl (%%rax), %%eax\n");
}

/* Store the value in %rax (of type ty) to the address in %rcx. */
static void store(Type *ty) {
    int sz = ty ? ty->size : 4;
    if (sz == 1)
        printf("    movb %%al, (%%rcx)\n");
    else if (sz == 8)
        printf("    movq %%rax, (%%rcx)\n");
    else
        printf("    movl %%eax, (%%rcx)\n");
}

/* generate address of lvalue into %rax */
static void gen_addr_x86(Node *node) {
    if (node->kind == ND_VAR) {
        if (node->var->is_global)
            printf("    leaq %s%s(%%rip), %%rax\n", sym_prefix, node->var->name);
        else
            printf("    leaq -%d(%%rbp), %%rax\n", node->var->offset);
        return;
    }
    if (node->kind == ND_DEREF) {
        gen_expr_x86(node->lhs);
        return;
    }
    /* struct member: address of base + member offset (resolved by add_type).
     * Covers s.x, p->x and nested combinations as assignment targets. */
    if (node->kind == ND_MEMBER) {
        gen_addr_x86(node->lhs);
        if (node->member && node->member->offset) printf("    addq $%d, %%rax\n", node->member->offset);
        return;
    }
    fprintf(stderr, "x86 codegen: not an lvalue (kind=%d)\n", node->kind);
    exit(1);
}

static void gen_expr_x86(Node *node) {
    if (!node) return;

    if (node->kind == ND_NUM) {
        printf("    movl $%d, %%eax\n", node->val);
        return;
    }

    /* variable load (width chosen from the annotated type) */
    if (node->kind == ND_VAR) {
        gen_addr_x86(node);
        load(node->ty ? node->ty : (node->var ? node->var->ty : NULL));
        return;
    }

    /* string literal */
    if (node->kind == ND_STR) {
        printf("    leaq %s_str_%d(%%rip), %%rax\n", lp, node->str_id);
        if (x86_nstrings < MAX_STRINGS) {
            x86_strings[x86_nstrings].id = node->str_id;
            x86_strings[x86_nstrings].val = node->str_val;
            x86_nstrings++;
        }
        return;
    }

    /* ternary ? : */
    if (node->kind == ND_COND) {
        int lbl = label_count++;
        gen_expr_x86(node->cond);
        printf("    testl %%eax, %%eax\n");
        printf("    je %sX_tern_f_%d\n", lp, lbl);
        gen_expr_x86(node->cond_true);
        printf("    jmp %sX_tern_end_%d\n", lp, lbl);
        printf("%sX_tern_f_%d:\n", lp, lbl);
        gen_expr_x86(node->cond_false);
        printf("%sX_tern_end_%d:\n", lp, lbl);
        return;
    }

    /* comma operator */
    if (node->kind == ND_COMMA) {
        gen_expr_x86(node->lhs);
        gen_expr_x86(node->rhs);
        return;
    }

    /* struct member access: shared address path + typed load */
    if (node->kind == ND_MEMBER) {
        gen_addr_x86(node);
        load(node->ty);
        return;
    }

    /* assignment: store width follows the lvalue's type */
    if (node->kind == ND_ASSIGN) {
        gen_addr_x86(node->lhs);
        push();
        gen_expr_x86(node->rhs);
        pop("%rcx");
        store(node->lhs->ty ? node->lhs->ty : (node->lhs->var ? node->lhs->var->ty : NULL));
        return;
    }

    /* variable declaration with initializer */
    if (node->kind == ND_VAR_DECL) {
        if (node->lhs) {
            gen_expr_x86(node->lhs);
            int sz = (node->var && node->var->ty) ? node->var->ty->size : 4;
            if (sz == 1)
                printf("    movb %%al, -%d(%%rbp)\n", node->var->offset);
            else if (sz == 8)
                printf("    movq %%rax, -%d(%%rbp)\n", node->var->offset);
            else
                printf("    movl %%eax, -%d(%%rbp)\n", node->var->offset);
        }
        return;
    }

    /* address-of */
    if (node->kind == ND_ADDR) {
        gen_addr_x86(node->lhs);
        return;
    }

    /* dereference: load width = the pointed-to type */
    if (node->kind == ND_DEREF) {
        gen_expr_x86(node->lhs);
        load(node->ty);
        return;
    }

    /* type cast */
    if (node->kind == ND_CAST) {
        gen_expr_x86(node->lhs);
        if (node->ty && node->ty->kind == TY_CHAR) printf("    movsbl %%al, %%eax\n");
        return;
    }

    /* function call (SysV: up to 6 integer register args) */
    if (node->kind == ND_CALL) {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) nargs++;
        if (nargs > 6) {
            fprintf(stderr,
                    "x86 codegen: call to '%s' with %d args — more than 6 "
                    "arguments are not supported\n",
                    node->name, nargs);
            exit(1);
        }
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr_x86(arg);
            push();
        }
        /* pop args into parameter registers (reverse order) */
        static const char *areg[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
        for (int i = nargs - 1; i >= 0; i--) pop(areg[i]);
        /* SysV alignment: pad 8 bytes when an odd number of pushes is live */
        int pad = depth % 2;
        if (pad) printf("    subq $8, %%rsp\n");
        /* variadic ABI: %al = number of vector-register args (always 0) */
        printf("    movb $0, %%al\n");
        printf("    call %s%s\n", sym_prefix, node->name);
        if (pad) printf("    addq $8, %%rsp\n");
        return;
    }

    /* unary operators */
    if (node->kind == ND_NEG) {
        gen_expr_x86(node->lhs);
        printf("    negl %%eax\n");
        return;
    }
    if (node->kind == ND_NOT) {
        gen_expr_x86(node->lhs);
        printf("    testl %%eax, %%eax\n");
        printf("    sete %%al\n");
        printf("    movzbl %%al, %%eax\n");
        return;
    }
    if (node->kind == ND_BITNOT) {
        gen_expr_x86(node->lhs);
        printf("    notl %%eax\n");
        return;
    }

    /* short-circuit && */
    if (node->kind == ND_AND) {
        int lbl = label_count++;
        gen_expr_x86(node->lhs);
        printf("    testl %%eax, %%eax\n");
        printf("    je %sX_false_%d\n", lp, lbl);
        gen_expr_x86(node->rhs);
        printf("    testl %%eax, %%eax\n");
        printf("    setne %%al\n");
        printf("    movzbl %%al, %%eax\n");
        printf("    jmp %sX_end_%d\n", lp, lbl);
        printf("%sX_false_%d:\n", lp, lbl);
        printf("    movl $0, %%eax\n");
        printf("%sX_end_%d:\n", lp, lbl);
        return;
    }

    /* short-circuit || */
    if (node->kind == ND_OR) {
        int lbl = label_count++;
        gen_expr_x86(node->lhs);
        printf("    testl %%eax, %%eax\n");
        printf("    jne %sX_true_%d\n", lp, lbl);
        gen_expr_x86(node->rhs);
        printf("    testl %%eax, %%eax\n");
        printf("    setne %%al\n");
        printf("    movzbl %%al, %%eax\n");
        printf("    jmp %sX_end_%d\n", lp, lbl);
        printf("%sX_true_%d:\n", lp, lbl);
        printf("    movl $1, %%eax\n");
        printf("%sX_end_%d:\n", lp, lbl);
        return;
    }

    /* binary operators: push rhs, compute lhs, pop rcx */
    gen_expr_x86(node->rhs);
    push();
    gen_expr_x86(node->lhs);
    pop("%rcx");

    /* pointer operands need 64-bit arithmetic / comparison on LP64 */
    int l_ptr = node->lhs && node->lhs->ty && is_pointer_like(node->lhs->ty);
    int r_ptr = node->rhs && node->rhs->ty && is_pointer_like(node->rhs->ty);

    switch (node->kind) {
        case ND_ADD:
            if (l_ptr || r_ptr) {
                /* ptr + scaled-int: sign-extend the int side, 64-bit add */
                printf("    movslq %%ecx, %%rcx\n");
                printf("    addq %%rcx, %%rax\n");
            } else {
                printf("    addl %%ecx, %%eax\n");
            }
            break;
        case ND_SUB:
            if (l_ptr && r_ptr) {
                /* ptr - ptr: full 64-bit difference (parser divides by size) */
                printf("    subq %%rcx, %%rax\n");
            } else if (l_ptr) {
                printf("    movslq %%ecx, %%rcx\n");
                printf("    subq %%rcx, %%rax\n");
            } else {
                printf("    subl %%ecx, %%eax\n");
            }
            break;
        case ND_MUL:
            printf("    imull %%ecx, %%eax\n");
            break;
        case ND_DIV:
            printf("    cltd\n");
            printf("    idivl %%ecx\n");
            break;
        case ND_MOD:
            printf("    cltd\n");
            printf("    idivl %%ecx\n");
            printf("    movl %%edx, %%eax\n");
            break;
        case ND_BITAND:
            printf("    andl %%ecx, %%eax\n");
            break;
        case ND_BITOR:
            printf("    orl %%ecx, %%eax\n");
            break;
        case ND_BITXOR:
            printf("    xorl %%ecx, %%eax\n");
            break;
        case ND_SHL:
            printf("    shll %%cl, %%eax\n");
            break;
        case ND_SHR:
            printf("    sarl %%cl, %%eax\n");
            break;
        case ND_EQ:
        case ND_NE:
        case ND_LT:
        case ND_LE:
        case ND_GT:
        case ND_GE: {
            if (l_ptr || r_ptr)
                printf("    cmpq %%rcx, %%rax\n");
            else
                printf("    cmpl %%ecx, %%eax\n");
            const char *cc = node->kind == ND_EQ   ? "sete"
                             : node->kind == ND_NE ? "setne"
                             : node->kind == ND_LT ? "setl"
                             : node->kind == ND_LE ? "setle"
                             : node->kind == ND_GT ? "setg"
                                                   : "setge";
            printf("    %s %%al\n", cc);
            printf("    movzbl %%al, %%eax\n");
            break;
        }
        default:
            fprintf(stderr, "x86 codegen: unexpected binary op %d\n", node->kind);
            exit(1);
    }
}

static void gen_stmt_x86(Node *node) {
    if (!node) return;

    switch (node->kind) {
        case ND_RETURN:
            if (node->lhs) gen_expr_x86(node->lhs);
            printf("    jmp %sX_return_%s\n", lp, cur_fn_name);
            return;

        case ND_IF: {
            int lbl = label_count++;
            gen_expr_x86(node->cond);
            printf("    testl %%eax, %%eax\n");
            printf("    je %sX_else_%d\n", lp, lbl);
            gen_stmt_x86(node->then);
            printf("    jmp %sX_endif_%d\n", lp, lbl);
            printf("%sX_else_%d:\n", lp, lbl);
            if (node->els) gen_stmt_x86(node->els);
            printf("%sX_endif_%d:\n", lp, lbl);
            return;
        }

        case ND_WHILE: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = lbl;
            loop_depth++;
            printf("%sX_while_%d:\n", lp, lbl);
            printf("%sX_cont_%d:\n", lp, lbl);
            gen_expr_x86(node->cond);
            printf("    testl %%eax, %%eax\n");
            printf("    je %sX_wend_%d\n", lp, lbl);
            gen_stmt_x86(node->body);
            printf("    jmp %sX_while_%d\n", lp, lbl);
            printf("%sX_wend_%d:\n", lp, lbl);
            printf("%sX_brk_%d:\n", lp, lbl);
            loop_depth--;
            return;
        }

        case ND_FOR: {
            int lbl = label_count++;
            int cont_lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = cont_lbl;
            loop_depth++;
            if (node->init) gen_stmt_x86(node->init);
            printf("%sX_for_%d:\n", lp, lbl);
            if (node->cond) {
                gen_expr_x86(node->cond);
                printf("    testl %%eax, %%eax\n");
                printf("    je %sX_fend_%d\n", lp, lbl);
            }
            gen_stmt_x86(node->body);
            printf("%sX_cont_%d:\n", lp, cont_lbl);
            if (node->inc) gen_expr_x86(node->inc);
            printf("    jmp %sX_for_%d\n", lp, lbl);
            printf("%sX_fend_%d:\n", lp, lbl);
            printf("%sX_brk_%d:\n", lp, lbl);
            loop_depth--;
            return;
        }

        case ND_DOWHILE: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = lbl;
            loop_depth++;
            printf("%sX_do_%d:\n", lp, lbl);
            printf("%sX_cont_%d:\n", lp, lbl);
            gen_stmt_x86(node->body);
            gen_expr_x86(node->cond);
            printf("    testl %%eax, %%eax\n");
            printf("    jne %sX_do_%d\n", lp, lbl);
            printf("%sX_brk_%d:\n", lp, lbl);
            loop_depth--;
            return;
        }

        case ND_SWITCH: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_depth++;
            gen_expr_x86(node->cond);
            int case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++) {
                if (c->kind == ND_CASE) {
                    printf("    cmpl $%d, %%eax\n", c->case_val);
                    printf("    je %sX_case_%d_%d\n", lp, lbl, case_num);
                }
            }
            int def_num = -1;
            case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++)
                if (c->kind == ND_DEFAULT) def_num = case_num;
            if (def_num >= 0)
                printf("    jmp %sX_case_%d_%d\n", lp, lbl, def_num);
            else
                printf("    jmp %sX_brk_%d\n", lp, lbl);
            /* generate case bodies (each body is a statement LIST) */
            case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++) {
                printf("%sX_case_%d_%d:\n", lp, lbl, case_num);
                for (Node *s = c->body; s; s = s->next) gen_stmt_x86(s);
            }
            printf("%sX_brk_%d:\n", lp, lbl);
            loop_depth--;
            return;
        }

        case ND_BREAK:
            if (loop_depth > 0) printf("    jmp %sX_brk_%d\n", lp, loop_break_labels[loop_depth - 1]);
            return;

        case ND_CONTINUE:
            if (loop_depth > 0) printf("    jmp %sX_cont_%d\n", lp, loop_cont_labels[loop_depth - 1]);
            return;

        case ND_BLOCK:
            for (Node *s = node->body; s; s = s->next) gen_stmt_x86(s);
            return;

        case ND_EXPR_STMT:
            gen_expr_x86(node->lhs);
            return;

        case ND_VAR_DECL:
            gen_expr_x86(node);
            return;

        default:
            fprintf(stderr, "x86 codegen: unexpected stmt %d\n", node->kind);
            exit(1);
    }
}

static void gen_function_x86(Node *fn) {
    if (fn->kind != ND_FUNC) return;

    if (fn->nparams > 6) {
        fprintf(stderr,
                "x86 codegen: function '%s' has %d parameters — more than 6 "
                "are not supported\n",
                fn->name, fn->nparams);
        exit(1);
    }

    cur_fn_name = fn->name;
    cur_fn_stack = (fn->stack_size + 15) & ~15; /* align to 16 */
    depth = 0;

    printf("\n.globl %s%s\n", sym_prefix, fn->name);
    printf("%s%s:\n", sym_prefix, fn->name);

    /* prologue: entry %rsp ≡ 8 (mod 16); push %rbp realigns to 16,
     * and the locals area is a multiple of 16, so the frame base is
     * 16-byte aligned — the call-site padding logic relies on this. */
    printf("    pushq %%rbp\n");
    printf("    movq %%rsp, %%rbp\n");
    printf("    subq $%d, %%rsp\n", cur_fn_stack);

    /* spill register parameters to their stack slots (width-aware:
     * a pointer parameter must be stored with all 64 bits) */
    static const char *preg64[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    static const char *preg32[] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
    static const char *preg8[] = {"%dil", "%sil", "%dl", "%cl", "%r8b", "%r9b"};
    int i = 0;
    for (Node *p = fn->params; p && i < 6; p = p->next, i++) {
        if (!p->var) continue;
        int sz = p->var->ty ? p->var->ty->size : 4;
        if (sz == 1)
            printf("    movb %s, -%d(%%rbp)\n", preg8[i], p->var->offset);
        else if (sz == 8)
            printf("    movq %s, -%d(%%rbp)\n", preg64[i], p->var->offset);
        else
            printf("    movl %s, -%d(%%rbp)\n", preg32[i], p->var->offset);
    }

    /* generate body */
    gen_stmt_x86(fn->body);

    /* epilogue */
    printf("%sX_return_%s:\n", lp, fn->name);
    printf("    leave\n");
    printf("    ret\n");
}

void codegen_x86(Node *prog) {
    /* resolve platform syntax (overridable via -target=x86-mac/x86-linux) */
    sym_prefix = x86_macos ? "_" : "";
    lp = x86_macos ? "L" : ".L";
    rodata_section = x86_macos ? ".section __TEXT,__cstring,cstring_literals" : ".section .rodata";
    str_directive = x86_macos ? ".asciz" : ".string";

    x86_nstrings = 0;

    /* .data section for global variables */
    int has_data = 0;
    for (Var *v = globals; v; v = v->next) {
        if (!has_data) {
            printf(".data\n");
            has_data = 1;
        }
        int al = (v->ty && v->ty->align) ? v->ty->align : 4;
        printf(".globl %s%s\n", sym_prefix, v->name);
        printf(".balign %d\n", al);
        printf("%s%s:\n", sym_prefix, v->name);
        if (v->ty && v->ty->kind == TY_ARRAY)
            printf("    .space %d\n", v->ty->size);
        else if (v->ty && v->ty->kind == TY_PTR)
            printf("    .quad %d\n", v->init_val); /* LP64 pointer slot */
        else
            printf("    .long %d\n", v->init_val);
    }

    /* .text section */
    printf("\n.text\n");
    for (Node *fn = prog->body; fn; fn = fn->next) gen_function_x86(fn);

    /* string literal section */
    if (x86_nstrings > 0) {
        printf("\n%s\n", rodata_section);
        for (int i = 0; i < x86_nstrings; i++) {
            printf("%s_str_%d:\n", lp, x86_strings[i].id);
            printf("    %s ", str_directive);
            emit_escaped_string(x86_strings[i].val);
            printf("\n");
        }
    }

    /* ELF only: mark the stack non-executable */
    if (!x86_macos) printf("\n.section .note.GNU-stack,\"\",@progbits\n");
}
