/*
 * codegen_arm.c - ARM (ARMv7) code generator for nccl-cc
 *
 * Lesson 94: ARM backend — same AST, different target.
 * Generates ARMv7 assembly (compatible with qemu-arm).
 *
 * RISC-V vs ARM mapping:
 *   a0-a7  →  r0-r7 (arguments)
 *   sp     →  sp (r13)
 *   ra     →  lr (r14)
 *   s0     →  fp (r11, frame pointer)
 *   ret    →  bx lr
 *   call   →  bl
 *   li     →  movw/movt pair
 *   lw/sw  →  ldr/str
 *   lb/sb  →  ldrb/strb
 *   beqz   →  cmp + beq
 *
 * Constants and symbol addresses are materialized with movw/movt
 * (ARMv7 16-bit immediate moves) instead of the `ldr rX, =value`
 * literal-pool pseudo: pools live in .text within ±4KB of the load,
 * and long generated functions overflow that range unless .ltorg
 * islands are sprinkled around — a footgun the fuzzer actually hit. */
#include "nccl_cc.h"

static int label_count = 0;
static const char *cur_fn_name;

/* break/continue loop context stack (mirrors codegen_rv.c) */
#define MAX_LOOP_DEPTH 32
static int loop_break_labels[MAX_LOOP_DEPTH];
static int loop_cont_labels[MAX_LOOP_DEPTH];
static int loop_depth = 0;

/* string literal storage for .rodata emission */
#define MAX_STRINGS 256
static struct {
    int id;
    char *val;
} arm_strings[MAX_STRINGS];
static int arm_nstrings = 0;

/* Load a 32-bit constant: movw sets the low half (zeroing the top),
 * movt fills the high half only when needed. */
static void load_imm(const char *reg, int val) {
    unsigned u = (unsigned)val;
    printf("    movw %s, #%u\n", reg, u & 0xffff);
    if (u >> 16) printf("    movt %s, #%u\n", reg, u >> 16);
}

/* Load the address of a symbol (global variable / string literal). */
static void load_sym_addr(const char *reg, const char *sym) {
    printf("    movw %s, #:lower16:%s\n", reg, sym);
    printf("    movt %s, #:upper16:%s\n", reg, sym);
}

/* Can v be encoded as an ARM data-processing immediate? Those are an
 * 8-bit value rotated right by an even amount - sub/add/cmp all use
 * this format, NOT the +-4095 of ldr/str. */
static int arm_imm_ok(unsigned v) {
    for (int rot = 0; rot < 32; rot += 2) {
        unsigned rotated = (rot == 0) ? v : ((v << rot) | (v >> (32 - rot)));
        if (rotated <= 0xff) return 1;
    }
    return 0;
}

/* Materialize `fp - offset` into reg. Offsets that don't fit the
 * rotated-immediate format go through ip (r12), the AAPCS scratch
 * register that survives nothing and clobbers nobody. */
static void addr_of_local(const char *reg, int offset) {
    if (arm_imm_ok((unsigned)offset)) {
        printf("    sub %s, fp, #%d\n", reg, offset);
    } else {
        load_imm("ip", offset);
        printf("    sub %s, fp, ip\n", reg);
    }
}

/* Store reg (width-aware) into the local at fp-offset. ldr/str
 * immediates are 12-bit (+-4095) - a DIFFERENT limit than sub's. */
static void store_local(const char *reg, int offset, int size) {
    const char *op = (size == 1) ? "strb" : "str";
    if (offset <= 4095) {
        printf("    %s %s, [fp, #-%d]\n", op, reg, offset);
    } else {
        addr_of_local("ip", offset);
        printf("    %s %s, [ip]\n", op, reg);
    }
}

static void push_arm(void) {
    printf("    sub sp, sp, #4\n");
    printf("    str r0, [sp]\n");
}

static void pop_arm(const char *reg) {
    printf("    ldr %s, [sp]\n", reg);
    printf("    add sp, sp, #4\n");
}

static void gen_expr_arm(Node *node);
static void gen_stmt_arm(Node *node);

/* Load a value of type ty from the address in r0 (char→ldrsb sign-
 * extended, int/pointer→4 bytes on ARM32). Arrays/structs: address only. */
static void load_val(Type *ty) {
    if (ty && (ty->kind == TY_ARRAY || ty->kind == TY_STRUCT)) return;
    if (ty && ty->size == 1)
        printf("    ldrsb r0, [r0]\n");
    else
        printf("    ldr r0, [r0]\n");
}

/* Store the value in r0 (of type ty) to the address in r1. */
static void store_val(Type *ty) {
    if (ty && ty->size == 1)
        printf("    strb r0, [r1]\n");
    else
        printf("    str r0, [r1]\n");
}

static void gen_addr_arm(Node *node) {
    if (node->kind == ND_VAR) {
        if (node->var->is_global)
            load_sym_addr("r0", node->var->name);
        else
            addr_of_local("r0", node->var->offset);
        return;
    }
    if (node->kind == ND_DEREF) {
        gen_expr_arm(node->lhs);
        return;
    }
    /* struct member as lvalue: base address + member offset */
    if (node->kind == ND_MEMBER) {
        gen_addr_arm(node->lhs);
        if (node->member && node->member->offset) printf("    add r0, r0, #%d\n", node->member->offset);
        return;
    }
    fprintf(stderr, "arm codegen: not an lvalue\n");
    exit(1);
}

static void gen_expr_arm(Node *node) {
    if (!node) return;

    if (node->kind == ND_NUM) {
        load_imm("r0", node->val);
        return;
    }

    /* variable load (width from the annotated type) */
    if (node->kind == ND_VAR) {
        gen_addr_arm(node);
        load_val(node->ty ? node->ty : (node->var ? node->var->ty : NULL));
        return;
    }

    /* string literal: load address from .rodata label */
    if (node->kind == ND_STR) {
        char lbl[32];
        snprintf(lbl, sizeof(lbl), ".LA_str_%d", node->str_id);
        load_sym_addr("r0", lbl);
        if (arm_nstrings < MAX_STRINGS) {
            arm_strings[arm_nstrings].id = node->str_id;
            arm_strings[arm_nstrings].val = node->str_val;
            arm_nstrings++;
        }
        return;
    }

    /* ternary ? : operator */
    if (node->kind == ND_COND) {
        int lbl = label_count++;
        gen_expr_arm(node->cond);
        printf("    cmp r0, #0\n");
        printf("    beq .LA_tern_f_%d\n", lbl);
        gen_expr_arm(node->cond_true);
        printf("    b .LA_tern_end_%d\n", lbl);
        printf(".LA_tern_f_%d:\n", lbl);
        gen_expr_arm(node->cond_false);
        printf(".LA_tern_end_%d:\n", lbl);
        return;
    }

    /* comma operator: evaluate both, return right */
    if (node->kind == ND_COMMA) {
        gen_expr_arm(node->lhs);
        gen_expr_arm(node->rhs);
        return;
    }

    /* struct member access: shared address path + typed load */
    if (node->kind == ND_MEMBER) {
        gen_addr_arm(node);
        load_val(node->ty);
        return;
    }

    /* assignment (store width follows the lvalue's type) */
    if (node->kind == ND_ASSIGN) {
        gen_addr_arm(node->lhs);
        push_arm();
        gen_expr_arm(node->rhs);
        pop_arm("r1");
        store_val(node->lhs->ty ? node->lhs->ty : (node->lhs->var ? node->lhs->var->ty : NULL));
        return;
    }

    /* variable declaration with initializer */
    if (node->kind == ND_VAR_DECL) {
        if (node->lhs) {
            gen_expr_arm(node->lhs);
            store_local("r0", node->var->offset, (node->var && node->var->ty) ? node->var->ty->size : 4);
        }
        return;
    }

    /* address-of */
    if (node->kind == ND_ADDR) {
        gen_addr_arm(node->lhs);
        return;
    }

    /* dereference (load width = the pointed-to type) */
    if (node->kind == ND_DEREF) {
        gen_expr_arm(node->lhs);
        load_val(node->ty);
        return;
    }

    /* type cast: (char)x sign-extends, matching ldrsb semantics */
    if (node->kind == ND_CAST) {
        gen_expr_arm(node->lhs);
        if (node->ty && node->ty->kind == TY_CHAR) printf("    sxtb r0, r0\n");
        return;
    }

    /* function call */
    if (node->kind == ND_CALL) {
        int nargs = 0;
        for (Node *arg = node->args; arg; arg = arg->next) {
            gen_expr_arm(arg);
            push_arm();
            nargs++;
        }
        static const char *areg[] = {"r0", "r1", "r2", "r3"};
        for (int i = nargs - 1; i >= 0; i--) {
            if (i < 4)
                pop_arm(areg[i]);
            else
                pop_arm("r4");
        }
        printf("    bl %s\n", node->name);
        return;
    }

    /* unary operators */
    if (node->kind == ND_NEG) {
        gen_expr_arm(node->lhs);
        printf("    rsb r0, r0, #0\n");
        return;
    }
    if (node->kind == ND_NOT) {
        gen_expr_arm(node->lhs);
        printf("    cmp r0, #0\n");
        printf("    moveq r0, #1\n");
        printf("    movne r0, #0\n");
        return;
    }
    if (node->kind == ND_BITNOT) {
        gen_expr_arm(node->lhs);
        printf("    mvn r0, r0\n");
        return;
    }

    /* short-circuit && */
    if (node->kind == ND_AND) {
        int lbl = label_count++;
        gen_expr_arm(node->lhs);
        printf("    cmp r0, #0\n");
        printf("    beq .LA_false_%d\n", lbl);
        gen_expr_arm(node->rhs);
        printf("    cmp r0, #0\n");
        printf("    movne r0, #1\n");
        printf("    b .LA_end_%d\n", lbl);
        printf(".LA_false_%d:\n", lbl);
        printf("    mov r0, #0\n");
        printf(".LA_end_%d:\n", lbl);
        return;
    }

    /* short-circuit || */
    if (node->kind == ND_OR) {
        int lbl = label_count++;
        gen_expr_arm(node->lhs);
        printf("    cmp r0, #0\n");
        printf("    bne .LA_true_%d\n", lbl);
        gen_expr_arm(node->rhs);
        printf("    cmp r0, #0\n");
        printf("    movne r0, #1\n");
        printf("    b .LA_end_%d\n", lbl);
        printf(".LA_true_%d:\n", lbl);
        printf("    mov r0, #1\n");
        printf(".LA_end_%d:\n", lbl);
        return;
    }

    /* binary ops: evaluate rhs first, push, then lhs */
    gen_expr_arm(node->rhs);
    push_arm();
    gen_expr_arm(node->lhs);
    pop_arm("r1");

    switch (node->kind) {
        case ND_ADD:
            printf("    add r0, r0, r1\n");
            break;
        case ND_SUB:
            printf("    sub r0, r0, r1\n");
            break;
        case ND_MUL:
            printf("    mul r0, r0, r1\n");
            break;
        case ND_DIV:
            printf("    sdiv r0, r0, r1\n");
            break;
        case ND_MOD:
            printf("    sdiv r2, r0, r1\n");
            printf("    mls r0, r2, r1, r0\n");
            break;
        case ND_BITAND:
            printf("    and r0, r0, r1\n");
            break;
        case ND_BITOR:
            printf("    orr r0, r0, r1\n");
            break;
        case ND_BITXOR:
            printf("    eor r0, r0, r1\n");
            break;
        case ND_SHL:
            printf("    lsl r0, r0, r1\n");
            break;
        case ND_SHR:
            printf("    asr r0, r0, r1\n");
            break;
        case ND_EQ:
            printf("    cmp r0, r1\n");
            printf("    moveq r0, #1\n");
            printf("    movne r0, #0\n");
            break;
        case ND_NE:
            printf("    cmp r0, r1\n");
            printf("    movne r0, #1\n");
            printf("    moveq r0, #0\n");
            break;
        case ND_LT:
            printf("    cmp r0, r1\n");
            printf("    movlt r0, #1\n");
            printf("    movge r0, #0\n");
            break;
        case ND_LE:
            printf("    cmp r0, r1\n");
            printf("    movle r0, #1\n");
            printf("    movgt r0, #0\n");
            break;
        case ND_GT:
            printf("    cmp r0, r1\n");
            printf("    movgt r0, #1\n");
            printf("    movle r0, #0\n");
            break;
        case ND_GE:
            printf("    cmp r0, r1\n");
            printf("    movge r0, #1\n");
            printf("    movlt r0, #0\n");
            break;
        default:
            fprintf(stderr, "arm codegen: unexpected binary op %d\n", node->kind);
            exit(1);
    }
}

static void gen_stmt_arm(Node *node) {
    if (!node) return;

    switch (node->kind) {
        case ND_RETURN:
            if (node->lhs) gen_expr_arm(node->lhs);
            printf("    b .LA_return_%s\n", cur_fn_name);
            return;

        case ND_IF: {
            int lbl = label_count++;
            gen_expr_arm(node->cond);
            printf("    cmp r0, #0\n");
            printf("    beq .LA_else_%d\n", lbl);
            gen_stmt_arm(node->then);
            printf("    b .LA_endif_%d\n", lbl);
            printf(".LA_else_%d:\n", lbl);
            if (node->els) gen_stmt_arm(node->els);
            printf(".LA_endif_%d:\n", lbl);
            return;
        }

        /* while — with break/continue support */
        case ND_WHILE: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = lbl;
            loop_depth++;
            printf(".LA_while_%d:\n", lbl);
            printf(".LA_cont_%d:\n", lbl);
            gen_expr_arm(node->cond);
            printf("    cmp r0, #0\n");
            printf("    beq .LA_wend_%d\n", lbl);
            gen_stmt_arm(node->body);
            printf("    b .LA_while_%d\n", lbl);
            printf(".LA_wend_%d:\n", lbl);
            printf(".LA_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        /* for — with break/continue support */
        case ND_FOR: {
            int lbl = label_count++;
            int cont_lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = cont_lbl;
            loop_depth++;
            if (node->init) gen_stmt_arm(node->init);
            printf(".LA_for_%d:\n", lbl);
            if (node->cond) {
                gen_expr_arm(node->cond);
                printf("    cmp r0, #0\n");
                printf("    beq .LA_fend_%d\n", lbl);
            }
            gen_stmt_arm(node->body);
            printf(".LA_cont_%d:\n", cont_lbl);
            if (node->inc) gen_expr_arm(node->inc);
            printf("    b .LA_for_%d\n", lbl);
            printf(".LA_fend_%d:\n", lbl);
            printf(".LA_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        /* do-while — with break/continue support */
        case ND_DOWHILE: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_cont_labels[loop_depth] = lbl;
            loop_depth++;
            printf(".LA_do_%d:\n", lbl);
            printf(".LA_cont_%d:\n", lbl);
            gen_stmt_arm(node->body);
            gen_expr_arm(node->cond);
            printf("    cmp r0, #0\n");
            printf("    bne .LA_do_%d\n", lbl);
            printf(".LA_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        /* switch/case: compare-and-branch for each case */
        case ND_SWITCH: {
            int lbl = label_count++;
            loop_break_labels[loop_depth] = lbl;
            loop_depth++;
            gen_expr_arm(node->cond);
            /* generate comparison jumps for each case */
            int case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++) {
                if (c->kind == ND_CASE) {
                    load_imm("r1", c->case_val);
                    printf("    cmp r0, r1\n");
                    printf("    beq .LA_case_%d_%d\n", lbl, case_num);
                }
            }
            /* jump to default or end */
            int def_num = -1;
            case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++)
                if (c->kind == ND_DEFAULT) def_num = case_num;
            if (def_num >= 0)
                printf("    b .LA_case_%d_%d\n", lbl, def_num);
            else
                printf("    b .LA_brk_%d\n", lbl);
            /* generate case bodies (each body is a statement LIST) */
            case_num = 0;
            for (Node *c = node->cases; c; c = c->next, case_num++) {
                printf(".LA_case_%d_%d:\n", lbl, case_num);
                for (Node *s = c->body; s; s = s->next) gen_stmt_arm(s);
            }
            printf(".LA_brk_%d:\n", lbl);
            loop_depth--;
            return;
        }

        case ND_BREAK:
            if (loop_depth > 0) printf("    b .LA_brk_%d\n", loop_break_labels[loop_depth - 1]);
            return;

        case ND_CONTINUE:
            if (loop_depth > 0) printf("    b .LA_cont_%d\n", loop_cont_labels[loop_depth - 1]);
            return;

        case ND_BLOCK:
            for (Node *s = node->body; s; s = s->next) gen_stmt_arm(s);
            return;

        case ND_EXPR_STMT:
            gen_expr_arm(node->lhs);
            return;

        case ND_VAR_DECL:
            gen_expr_arm(node);
            return;

        default:
            fprintf(stderr, "arm codegen: unexpected stmt %d\n", node->kind);
            exit(1);
    }
}

static void gen_function_arm(Node *fn) {
    if (fn->kind != ND_FUNC) return;

    cur_fn_name = fn->name;

    printf("\n.globl %s\n", fn->name);
    printf("%s:\n", fn->name);

    /* prologue */
    printf("    push {fp, lr}\n");
    printf("    mov fp, sp\n");
    if (arm_imm_ok((unsigned)fn->stack_size)) {
        printf("    sub sp, sp, #%d\n", fn->stack_size);
    } else {
        load_imm("ip", fn->stack_size);
        printf("    sub sp, sp, ip\n");
    }

    /* store params r0-r3 (width-aware) */
    int i = 0;
    for (Node *p = fn->params; p && i < 4; p = p->next, i++) {
        if (!p->var) continue;
        char reg[4];
        snprintf(reg, sizeof(reg), "r%d", i);
        store_local(reg, p->var->offset, (p->var->ty) ? p->var->ty->size : 4);
    }

    gen_stmt_arm(fn->body);

    /* epilogue */
    printf(".LA_return_%s:\n", fn->name);
    printf("    mov sp, fp\n");
    printf("    pop {fp, pc}\n");
}

void codegen_arm(Node *prog) {
    arm_nstrings = 0;

    /* .data section for global variables */
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

    /* .text section for functions.
     * armv7ve includes the integer divide extension — plain armv7-a
     * would reject the sdiv instruction at assembly time. */
    printf("\n.text\n");
    printf(".syntax unified\n");
    printf(".arch armv7ve\n");
    for (Node *fn = prog->body; fn; fn = fn->next) gen_function_arm(fn);

    /* .rodata section for string literals */
    if (arm_nstrings > 0) {
        printf("\n.section .rodata\n");
        for (int i = 0; i < arm_nstrings; i++) {
            printf(".LA_str_%d:\n", arm_strings[i].id);
            printf("    .string ");
            emit_escaped_string(arm_strings[i].val);
            printf("\n");
        }
    }

    /* non-executable stack note ('@' starts a comment in ARM asm,
     * so the type must be spelled %progbits here) */
    printf("\n.section .note.GNU-stack,\"\",%%progbits\n");
}
