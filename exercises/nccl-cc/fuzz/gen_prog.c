/*
 * gen_prog.c - random C-subset program generator for differential
 * fuzzing of nccl-cc.
 *
 *     cc -o gen_prog gen_prog.c
 *     ./gen_prog <seed>            # deterministic program on stdout
 *
 * The generated program is valid under BOTH the host gcc (compiled
 * with -fwrapv) and every nccl-cc backend, and always terminates.
 * Differential testing then compares process exit codes.
 *
 * Undefined behavior is designed out, not hoped away:
 *   - division/modulo: divisor is always ((expr & 15) + 1)  -> 1..16,
 *     never zero, never -1 (kills both div-by-zero and INT_MIN/-1)
 *   - shifts: count is (expr & 7); left-shift operand is masked to
 *     (expr & 1023) so no negative value is ever shifted left
 *   - signed overflow: + - * wrap identically on every backend
 *     (2's-complement machine ops); the gcc REFERENCE build uses
 *     -fwrapv so the comparison is apples-to-apples
 *   - loops: generator-owned counter with a fixed bound, the loop
 *     body cannot touch it (it is not in the variable pool)
 *   - arrays: every index is masked to the 16-element bound (& 15)
 *   - pointers: pN is repointed only into the FIRST HALF of its
 *     array (base & 7) and dereferenced at most base+7 away, so
 *     *(pN + k) stays inside arrN by construction
 *
 * The PRNG is a self-contained xorshift32: identical sequences on
 * every host, no libc rand() portability traps.
 */
#include <stdio.h>
#include <stdlib.h>

/*===== deterministic PRNG =====*/
static unsigned rng;

static unsigned rnd(void) {
    rng ^= rng << 13;
    rng ^= rng >> 17;
    rng ^= rng << 5;
    return rng;
}

/* uniform value in [lo, hi] */
static int rnd_range(int lo, int hi) { return lo + (int)(rnd() % (unsigned)(hi - lo + 1)); }

/* true with probability pct/100 */
static int chance(int pct) { return rnd_range(1, 100) <= pct; }

/*===== variable pool =====*/
#define NLOCALS 6
#define NGLOBALS 3
#define NARRAYS 3 /* arr0, arr1 local; garr global - all int[16] */
#define NPTRS 2   /* p0 -> arr0, p1 -> arr1 (repointed, see rules) */

static const char *pick_var(void) {
    static char buf[8];
    if (chance(25))
        snprintf(buf, sizeof(buf), "g%d", rnd_range(0, NGLOBALS - 1));
    else
        snprintf(buf, sizeof(buf), "v%d", rnd_range(0, NLOCALS - 1));
    return buf;
}

static const char *pick_arr(void) {
    static const char *names[] = {"arr0", "arr1", "garr"};
    return names[rnd_range(0, NARRAYS - 1)];
}

/*===== expression generator =====*/
static void gen_expr(int depth);

/* arrN[(e) & 15] - always in bounds */
static void gen_arr_read(int depth) {
    printf("%s[(", pick_arr());
    gen_expr(depth);
    printf(") & 15]");
}

/* *(pN + ((e) & 7)) or pN[(e) & 7] - pN sits in the first half of
 * its array, so base(<=8) + 7 <= 15 stays in bounds */
static void gen_ptr_read(int depth) {
    int p = rnd_range(0, NPTRS - 1);
    if (chance(50)) {
        printf("(*(p%d + ((", p);
        gen_expr(depth);
        printf(") & 7)))");
    } else {
        printf("p%d[(", p);
        gen_expr(depth);
        printf(") & 7]");
    }
}

static void gen_leaf(void) {
    if (chance(45))
        printf("%d", rnd_range(-64, 64));
    else
        printf("%s", pick_var());
}

static void gen_expr(int depth) {
    if (depth <= 0) {
        gen_leaf();
        return;
    }

    /* memory reads appear at any depth > 0 */
    if (chance(15)) {
        if (chance(60))
            gen_arr_read(depth - 1);
        else
            gen_ptr_read(depth - 1);
        return;
    }

    switch (rnd_range(0, 11)) {
        case 0:
        case 1: { /* basic arithmetic: + - * */
            static const char *ops[] = {"+", "-", "*"};
            const char *op = ops[rnd_range(0, 2)];
            printf("(");
            gen_expr(depth - 1);
            printf(" %s ", op);
            gen_expr(depth - 1);
            printf(")");
            break;
        }
        case 2: { /* comparison */
            static const char *ops[] = {"<", "<=", ">", ">=", "==", "!="};
            const char *op = ops[rnd_range(0, 5)];
            printf("(");
            gen_expr(depth - 1);
            printf(" %s ", op);
            gen_expr(depth - 1);
            printf(")");
            break;
        }
        case 3: { /* short-circuit logic */
            printf("(");
            gen_expr(depth - 1);
            printf(" %s ", chance(50) ? "&&" : "||");
            gen_expr(depth - 1);
            printf(")");
            break;
        }
        case 4: { /* bitwise */
            static const char *ops[] = {"&", "|", "^"};
            const char *op = ops[rnd_range(0, 2)];
            printf("(");
            gen_expr(depth - 1);
            printf(" %s ", op);
            gen_expr(depth - 1);
            printf(")");
            break;
        }
        case 5: /* safe division */
            printf("(");
            gen_expr(depth - 1);
            printf(" / ((");
            gen_expr(depth - 1);
            printf(" & 15) + 1))");
            break;
        case 6: /* safe modulo */
            printf("(");
            gen_expr(depth - 1);
            printf(" %% ((");
            gen_expr(depth - 1);
            printf(" & 15) + 1))");
            break;
        case 7: /* safe left shift: non-negative operand, count 0..7 */
            printf("(((");
            gen_expr(depth - 1);
            printf(") & 1023) << (");
            gen_expr(depth - 1);
            printf(" & 7))");
            break;
        case 8: /* right shift, count 0..7 */
            printf("((");
            gen_expr(depth - 1);
            printf(") >> (");
            gen_expr(depth - 1);
            printf(" & 7))");
            break;
        case 9: { /* unary */
            static const char *ops[] = {"-", "!", "~"};
            printf("%s(", ops[rnd_range(0, 2)]);
            gen_expr(depth - 1);
            printf(")");
            break;
        }
        case 10: /* ternary */
            printf("((");
            gen_expr(depth - 1);
            printf(") ? (");
            gen_expr(depth - 1);
            printf(") : (");
            gen_expr(depth - 1);
            printf("))");
            break;
        case 11: /* function call through the fixed helper */
            printf("mix(");
            gen_expr(depth - 1);
            printf(", ");
            gen_expr(depth - 1);
            printf(")");
            break;
    }
}

/*===== statement generator =====*/
static int loop_counter; /* unique name for generator-owned loop vars */

static void indent(int n) {
    for (int i = 0; i < n; i++) printf("    ");
}

static void gen_stmt(int depth, int ind);

static void gen_block(int depth, int ind, int min_stmts, int max_stmts) {
    int n = rnd_range(min_stmts, max_stmts);
    for (int i = 0; i < n; i++) gen_stmt(depth, ind);
}

static void gen_stmt(int depth, int ind) {
    int kind = rnd_range(0, 12);

    /* keep nesting bounded */
    if (depth <= 0 && kind >= 10) kind = rnd_range(0, 9);

    switch (kind) {
        case 0:
        case 1:
        case 2: { /* plain assignment */
            indent(ind);
            printf("%s = ", pick_var());
            gen_expr(rnd_range(1, 3));
            printf(";\n");
            break;
        }
        case 3: { /* compound assignment (lvalue evaluated once!) */
            static const char *ops[] = {"+=", "-=", "*=", "&=", "|=", "^="};
            indent(ind);
            printf("%s %s ", pick_var(), ops[rnd_range(0, 5)]);
            gen_expr(rnd_range(1, 2));
            printf(";\n");
            break;
        }
        case 4: { /* safe compound divide */
            indent(ind);
            printf("%s /= ((", pick_var());
            gen_expr(1);
            printf(" & 15) + 1);\n");
            break;
        }
        case 5: { /* ++/-- in all four shapes */
            static const char *shapes[] = {"%s++;\n", "%s--;\n", "++%s;\n", "--%s;\n"};
            indent(ind);
            printf(shapes[rnd_range(0, 3)], pick_var());
            break;
        }
        case 6: { /* expression statement (value discarded) */
            indent(ind);
            gen_expr(rnd_range(1, 2));
            printf(";\n");
            break;
        }
        case 7: { /* array element write / compound write / ++ */
            const char *a = pick_arr();
            int form = rnd_range(0, 3);
            indent(ind);
            printf("%s[(", a);
            gen_expr(1);
            if (form == 0) {
                printf(") & 15] = ");
                gen_expr(rnd_range(1, 2));
                printf(";\n");
            } else if (form == 1) {
                static const char *ops[] = {"+=", "-=", "*=", "^="};
                printf(") & 15] %s ", ops[rnd_range(0, 3)]);
                gen_expr(rnd_range(1, 2));
                printf(";\n");
            } else if (form == 2) {
                printf(") & 15]++;\n");
            } else {
                printf(") & 15]--;\n");
            }
            break;
        }
        case 8: { /* pointer write or repoint (stays in the safe half) */
            int p = rnd_range(0, NPTRS - 1);
            indent(ind);
            if (chance(60)) {
                printf("*(p%d + ((", p);
                gen_expr(1);
                printf(") & 7)) = ");
                gen_expr(rnd_range(1, 2));
                printf(";\n");
            } else {
                /* p0 may roam arr0[0..7], p1 roams arr1[0..7] */
                printf("p%d = arr%d + ((", p, p);
                gen_expr(1);
                printf(") & 7);\n");
            }
            break;
        }
        case 9: { /* expression statement reading memory */
            indent(ind);
            if (chance(50))
                gen_arr_read(1);
            else
                gen_ptr_read(1);
            printf(";\n");
            break;
        }
        case 10:
        case 11: { /* if / if-else */
            indent(ind);
            printf("if (");
            gen_expr(2);
            printf(") {\n");
            gen_block(depth - 1, ind + 1, 1, 3);
            indent(ind);
            printf("}");
            if (chance(50)) {
                printf(" else {\n");
                gen_block(depth - 1, ind + 1, 1, 3);
                indent(ind);
                printf("}");
            }
            printf("\n");
            break;
        }
        case 12: { /* bounded while loop: counter owned by the generator,
                    * NOT in the variable pool, so the body can't corrupt it */
            int lv = loop_counter++;
            int bound = rnd_range(2, 8);
            indent(ind);
            printf("int l%d = 0;\n", lv);
            indent(ind);
            printf("while (l%d < %d) {\n", lv, bound);
            gen_block(depth - 1, ind + 1, 1, 3);
            indent(ind + 1);
            printf("l%d++;\n", lv);
            indent(ind);
            printf("}\n");
            break;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <seed>\n", argv[0]);
        return 2;
    }
    rng = (unsigned)strtoul(argv[1], NULL, 0);
    if (rng == 0) rng = 0x9e3779b9; /* xorshift must not start at 0 */

    printf("/* generated by fuzz/gen_prog.c, seed %s */\n", argv[1]);

    /* globals */
    for (int i = 0; i < NGLOBALS; i++) printf("int g%d;\n", i);
    printf("int garr[16];\n");

    /* fixed helper: exercises the call path; wrap-around mul is fine
     * because the reference build uses -fwrapv */
    printf("\nint mix(int a, int b) {\n");
    printf("    return a * 31 + b;\n");
    printf("}\n");

    printf("\nint main(void) {\n");
    for (int i = 0; i < NLOCALS; i++) printf("    int v%d = %d;\n", i, rnd_range(-50, 50));
    for (int i = 0; i < NGLOBALS; i++) printf("    g%d = %d;\n", i, rnd_range(-50, 50));

    /* arrays + the pointers that roam them (deterministic init so
     * every read after this point is of a defined value; garr is
     * .bss-zero already but gets filled too for more variety) */
    printf("    int arr0[16];\n");
    printf("    int arr1[16];\n");
    printf("    int fz = 0;\n");
    printf("    while (fz < 16) {\n");
    printf("        arr0[fz] = fz * %d;\n", rnd_range(-9, 9));
    printf("        arr1[fz] = fz - %d;\n", rnd_range(0, 15));
    printf("        garr[fz] = fz * fz;\n");
    printf("        fz = fz + 1;\n");
    printf("    }\n");
    printf("    int *p0 = arr0;\n");
    printf("    int *p1 = arr1 + %d;\n", rnd_range(0, 7));
    printf("\n");

    gen_block(2, 1, 8, 14);

    /* fold every variable AND the memory state into the exit code;
     * & 255 keeps it inside the byte that wait() exposes, so gcc and
     * qemu agree exactly */
    printf("\n    int fs = 0;\n");
    printf("    fz = 0;\n");
    printf("    while (fz < 16) {\n");
    printf("        fs = fs * 31 + arr0[fz] + arr1[fz] - garr[fz];\n");
    printf("        fz = fz + 1;\n");
    printf("    }\n");
    printf("    return (fs + *p0 + *p1");
    for (int i = 0; i < NLOCALS; i++) printf(" + v%d", i);
    for (int i = 0; i < NGLOBALS; i++) printf(" + g%d", i);
    printf(") & 255;\n");
    printf("}\n");
    return 0;
}
