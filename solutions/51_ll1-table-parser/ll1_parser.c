/* 51_ll1-table-parser.c — LL(1) 预测分析表构造与语法解析 (SOLUTION) */
#include <stdio.h>
#include <string.h>

#define STACK_SZ 64
#define PROD_CNT 8

/* ─── 符号定义 ─── */
enum { T_ID, T_PLUS, T_STAR, T_LPAR, T_RPAR, T_EOF, T_COUNT = 6 };
enum { NT_E, NT_Ee, NT_T, NT_Te, NT_F, NT_COUNT = 5 };
enum { SYM_TERM, SYM_NONTERM };

typedef struct {
    int type;
    int id;
} Sym;

const char *tname[T_COUNT] = {"id", "+", "*", "(", ")", "$"};
const char *nname[NT_COUNT] = {"E", "E'", "T", "T'", "F"};

const char *prod_label[PROD_CNT] = {
    "E  → T E'", "E' → + T E'", "E' → ε", "T  → F T'", "T' → ε", "T' → * F T'", "F  → id", "F  → ( E )",
};

#define NT(x) ((x) + 100)
int rhs[PROD_CNT][4] = {
    {NT(NT_Ee), NT(NT_T), -1},
    {NT(NT_Ee), NT(NT_T), T_PLUS, -1},
    {-1},
    {NT(NT_Te), NT(NT_F), -1},
    {-1},
    {NT(NT_Te), NT(NT_F), T_STAR, -1},
    {T_ID, -1},
    {T_RPAR, NT(NT_E), T_LPAR, -1},
};

int prod_lhs[PROD_CNT] = {NT_E, NT_Ee, NT_Ee, NT_T, NT_Te, NT_Te, NT_F, NT_F};

int first[NT_COUNT];
int follow[NT_COUNT];
int table[NT_COUNT][T_COUNT];

#define HAS(bitmask, t) ((bitmask) & (1 << (t)))
#define ADD(bitmask, t) ((bitmask) |= (1 << (t)))

static int rhs_len(int p) {
    int len = 0;
    while (rhs[p][len] != -1) len++;
    return len;
}

static int rhs_at(int p, int i) {
    int len = rhs_len(p);
    return rhs[p][len - 1 - i];
}

static int is_terminal(int sym_id) { return sym_id >= 0 && sym_id < T_COUNT; }

static void input_str(int *tokens, int pos, int n, char *buf) {
    int p = 0;
    for (int i = pos; i < n; i++) {
        const char *s = tname[tokens[i]];
        while (*s) buf[p++] = *s++;
    }
    buf[p] = '\0';
}

static void set_str(int mask, char *buf) {
    int p = 0;
    buf[p++] = '{';
    int first_item = 1;
    for (int t = 0; t < T_COUNT; t++) {
        if (HAS(mask, t)) {
            if (!first_item) {
                buf[p++] = ',';
                buf[p++] = ' ';
            }
            first_item = 0;
            const char *s = tname[t];
            while (*s) buf[p++] = *s++;
        }
    }
    buf[p++] = '}';
    buf[p] = '\0';
}

/* TODO 1: token_id */
static int token_id(const char *s) {
    for (int i = 0; i < T_COUNT; i++)
        if (!strcmp(s, tname[i])) return i;
    return -1;
}

/* TODO 2: 栈操作 */
Sym stack[STACK_SZ];
int top_idx = -1;

static void push(int type, int id) {
    Sym s = {type, id};
    stack[++top_idx] = s;
}
static Sym pop(void) { return stack[top_idx--]; }
static int empty(void) { return top_idx < 0; }
static void stack_str(char *buf) {
    int p = 0;
    for (int i = 0; i <= top_idx; i++) {
        Sym *s = &stack[i];
        const char *nm = s->type == SYM_TERM ? tname[s->id] : nname[s->id];
        while (*nm) buf[p++] = *nm++;
    }
    buf[p] = '\0';
}

/* TODO 3: compute_first */
static void compute_first(void) {
    int changed;
    do {
        changed = 0;
        for (int p = 0; p < PROD_CNT; p++) {
            int A = prod_lhs[p];
            int len = rhs_len(p);
            if (len == 0) continue;

            for (int i = 0; i < len; i++) {
                int X = rhs_at(p, i);

                if (is_terminal(X)) {
                    int before = first[A];
                    ADD(first[A], X);
                    if (first[A] != before) changed = 1;
                    break;
                } else {
                    int X_id = X - 100;
                    int before = first[A];
                    for (int t = 0; t < T_COUNT; t++) {
                        if (HAS(first[X_id], t)) ADD(first[A], t);
                    }
                    if (first[A] != before) changed = 1;
                    if (X_id != NT_Ee && X_id != NT_Te) break;
                }
            }
        }
    } while (changed);
}

/* TODO 4: compute_follow */
static void compute_follow(void) {
    ADD(follow[NT_E], T_EOF);

    int changed;
    do {
        changed = 0;
        for (int p = 0; p < PROD_CNT; p++) {
            int A = prod_lhs[p];
            int len = rhs_len(p);

            for (int i = 0; i < len; i++) {
                int X = rhs_at(p, i);
                if (is_terminal(X)) continue;
                int B = X - 100;

                int beta_all_nullable = 1;
                for (int j = i + 1; j < len; j++) {
                    int Y = rhs_at(p, j);

                    if (is_terminal(Y)) {
                        int before = follow[B];
                        ADD(follow[B], Y);
                        if (follow[B] != before) changed = 1;
                        beta_all_nullable = 0;
                        break;
                    } else {
                        int Y_id = Y - 100;
                        int before = follow[B];
                        for (int t = 0; t < T_COUNT; t++) {
                            if (HAS(first[Y_id], t)) ADD(follow[B], t);
                        }
                        if (follow[B] != before) changed = 1;
                        if (Y_id != NT_Ee && Y_id != NT_Te) {
                            beta_all_nullable = 0;
                            break;
                        }
                    }
                }

                if (beta_all_nullable) {
                    int before = follow[B];
                    for (int t = 0; t < T_COUNT; t++) {
                        if (HAS(follow[A], t)) ADD(follow[B], t);
                    }
                    if (follow[B] != before) changed = 1;
                }
            }
        }
    } while (changed);
}

/* TODO 5: build_table */
static void build_table(void) {
    for (int nt = 0; nt < NT_COUNT; nt++)
        for (int t = 0; t < T_COUNT; t++) table[nt][t] = -1;

    for (int p = 0; p < PROD_CNT; p++) {
        int A = prod_lhs[p];
        int len = rhs_len(p);

        if (len == 0) {
            for (int t = 0; t < T_COUNT; t++) {
                if (HAS(follow[A], t)) table[A][t] = p;
            }
            continue;
        }

        int nullable_prod = 1;
        for (int i = 0; i < len; i++) {
            int X = rhs_at(p, i);

            if (is_terminal(X)) {
                table[A][X] = p;
                nullable_prod = 0;
                break;
            } else {
                int X_id = X - 100;
                for (int t = 0; t < T_COUNT; t++) {
                    if (HAS(first[X_id], t)) table[A][t] = p;
                }
                if (X_id != NT_Ee && X_id != NT_Te) {
                    nullable_prod = 0;
                    break;
                }
            }
        }

        if (nullable_prod) {
            for (int t = 0; t < T_COUNT; t++) {
                if (HAS(follow[A], t)) table[A][t] = p;
            }
        }
    }
}

/* TODO 6: print_sets */
static void print_sets(void) {
    char buf[64];
    printf("=== FIRST Sets ===\n");
    for (int nt = 0; nt < NT_COUNT; nt++) {
        set_str(first[nt], buf);
        printf("FIRST(%s) = %s\n", nname[nt], buf);
    }
    printf("\n=== FOLLOW Sets ===\n");
    for (int nt = 0; nt < NT_COUNT; nt++) {
        set_str(follow[nt], buf);
        printf("FOLLOW(%s) = %s\n", nname[nt], buf);
    }
    printf("\n");
}

/* 主解析循环 */
int main(void) {
    char *input[] = {"id", "+", "id", "*", "id", "$", NULL};
    int tokens[16], n = 0;
    for (int i = 0; input[i]; i++) tokens[n++] = token_id(input[i]);

    compute_first();
    compute_follow();
    build_table();

    printf("=== LL(1) Table-Driven Parser ===\n");
    printf("Grammar: E→TE' E'→+TE'|ε T→FT' T'→*FT'|ε F→id|(E)\n");
    printf("Input:   id + id * id\n\n");

    print_sets();

    push(SYM_TERM, T_EOF);
    push(SYM_NONTERM, NT_E);

    int ip = 0, step = 0;
    char s_buf[64], i_buf[64];

    while (!empty()) {
        Sym s_top = stack[top_idx];
        int cur = tokens[ip];
        stack_str(s_buf);
        input_str(tokens, ip, n, i_buf);
        printf("[%02d] %-18s %-16s ", step, s_buf, i_buf);

        if (s_top.type == SYM_TERM) {
            if (s_top.id == cur) {
                printf("match %s\n", tname[cur]);
                pop();
                ip++;
            } else {
                printf("ERROR\n");
                return 1;
            }
        } else {
            int pid = table[s_top.id][cur];
            if (pid < 0) {
                printf("ERROR\n");
                return 1;
            }
            printf("%s\n", prod_label[pid]);
            pop();
            for (int i = 0; rhs[pid][i] != -1; i++) {
                int r = rhs[pid][i];
                if (r >= 100)
                    push(SYM_NONTERM, r - 100);
                else
                    push(SYM_TERM, r);
            }
        }
        step++;
    }
    stack_str(s_buf);
    input_str(tokens, ip, n, i_buf);
    printf("[%02d] %-18s %-16s ACCEPT\n", step, s_buf, i_buf);
    return 0;
}
