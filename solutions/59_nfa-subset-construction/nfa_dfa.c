/* 59_nfa-subset-construction/nfa_dfa.c — NFA 模拟与子集构造→DFA
 *
 * 任务：1. e_closure()      — 计算ε-闭包
 *       2. NFA_simulate()   — NFA 模拟 (给定输入串判断是否接受)
 *       3. subset_construct() — 子集构造 (NFA→DFA)
 *       4. main()           — 主流程
 *
 * 固定 NFA: 识别语言 a*b | ab*
 *   状态 0-3, 字母表{a,b}, 含ε转移
 *   ε: 0→1, 0→2
 *   a: 1→1, 2→3
 *   b: 1→3, 3→3
 *   初态：0, 接受态：3
 *
 * 知识点：ε-闭包、NFA 模拟、子集构造、DFA 最小化
 *
 * 验证：make test 比对 expected_output.txt
 */
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_STATES 4
#define MAX_SYMBOLS 2
#define MAX_SUBSETS 16 /* 2^4 = 16 possible subsets */

/* 符号映射：'a' -> 0, 'b' -> 1 */
static int sym_idx(char c) { return (c == 'a') ? 0 : 1; }

/* NFA 转移表：NFA_TRANS[state][symbol] = {targets, -1 terminated}
 * 约定：-1 表示ε转移，-2 表示无转移/列表终止
 * 使用负数标记ε: 对于 symbol 维度，我们用单独数组存ε转移 */
#define NO_TARGET -2
#define EPSILON -1

/* NFA 转移：trans[state][symbol] = 可达状态列表，以 NO_TARGET 结束 */
static const int NFA_TRANS[4][2][4] = {
    /* state 0 */ {{NO_TARGET}, {NO_TARGET}},       /* 0 只有ε转移 */
    /* state 1 */ {{1, NO_TARGET}, {3, NO_TARGET}}, /* a->1, b->3 */
    /* state 2 */ {{3, NO_TARGET}, {NO_TARGET}},    /* a->3 */
    /* state 3 */ {{NO_TARGET}, {3, NO_TARGET}},    /* b->3 */
};

/* ε转移：epsilon[state] = {可达状态列表，NO_TARGET 结束} */
static const int EPSILON_TRANS[4][4] = {
    {1, 2, NO_TARGET}, /* 0 -> 1, 2 */
    {NO_TARGET},       /* 1: 无ε转移 */
    {NO_TARGET},       /* 2: 无ε转移 */
    {NO_TARGET},       /* 3: 无ε转移 */
};

/* ─── ε-闭包计算 ───
 * 给定状态集 (set, 用位掩码表示), 返回包含所有ε可达状态的位掩码 */
static int e_closure(int states) {
    int closure = states;
    int changed;
    do {
        changed = 0;
        for (int s = 0; s < MAX_STATES; s++) {
            if (closure & (1 << s)) {
                for (int t = 0; EPSILON_TRANS[s][t] != NO_TARGET; t++) {
                    int target = EPSILON_TRANS[s][t];
                    if (!(closure & (1 << target))) {
                        closure |= (1 << target);
                        changed = 1;
                    }
                }
            }
        }
    } while (changed);
    return closure;
}

/* ─── 打印状态集 (用于调试输出) ─── */
static void print_state_set(int states, FILE *fp) {
    fprintf(fp, "{");
    int first = 1;
    for (int s = 0; s < MAX_STATES; s++) {
        if (states & (1 << s)) {
            if (!first) fprintf(fp, ",");
            fprintf(fp, "%d", s);
            first = 0;
        }
    }
    fprintf(fp, "}");
}

/* ─── NFA 模拟 ───
 * 给定输入串，返回 true(接受) 或 false(拒绝), 同时打印每步状态集 */
static bool NFA_simulate(const char *input, bool verbose) {
    int current = e_closure(1 << 0); /* start from state 0, take ε-closure */

    if (verbose) {
        printf("NFA simulation for \"%s\":\n", input);
        printf("  Start ε-closure({0}) = ");
        print_state_set(current, stdout);
        printf("\n");
    }

    for (int i = 0; input[i] != '\0'; i++) {
        char c = input[i];
        int si = sym_idx(c);
        int next = 0;

        /* 对当前状态集中的每个状态，计算 symbol 转移 */
        for (int s = 0; s < MAX_STATES; s++) {
            if (current & (1 << s)) {
                for (int t = 0; NFA_TRANS[s][si][t] != NO_TARGET; t++) {
                    int target = NFA_TRANS[s][si][t];
                    next |= (1 << target);
                }
            }
        }

        /* 计算ε-闭包 */
        next = e_closure(next);

        if (verbose) {
            printf("  Read '%c': ", c);
            print_state_set(current, stdout);
            printf(" --%c--> ", c);
            print_state_set(next, stdout);
            printf("\n");
        }

        current = next;
    }

    /* 检查是否在接受态 */
    bool accepted = (current & (1 << 3)) != 0;
    if (verbose) {
        printf("  Final states: ");
        print_state_set(current, stdout);
        printf(" → %s\n", accepted ? "ACCEPT" : "REJECT");
    }
    return accepted;
}

/* ─── 子集构造 NFA→DFA ───
 * 从 NFA 构造等价的 DFA, 打印 DFA 转换表 */
static void subset_construct(void) {
    /* DFA 状态：每个 DFA 状态对应一个 NFA 状态子集 (位掩码) */
    int dfa_states[MAX_SUBSETS];   /* NFA 子集 */
    int dfa_trans[MAX_SUBSETS][2]; /* DFA 转移表 */
    int dfa_count = 0;
    int dfa_accepting[MAX_SUBSETS]; /* 是否为接受态 */

    /* 初始化 */
    for (int i = 0; i < MAX_SUBSETS; i++) {
        dfa_states[i] = -1;
        dfa_trans[i][0] = -1;
        dfa_trans[i][1] = -1;
        dfa_accepting[i] = 0;
    }

    /* DFA 初态 = ε-closure({0}) */
    int start_subset = e_closure(1 << 0);
    dfa_states[0] = start_subset;
    dfa_count = 1;

    /* 处理所有 DFA 状态 */
    for (int d = 0; d < dfa_count; d++) {
        int subset = dfa_states[d];

        /* 标记接受态 */
        if (subset & (1 << 3)) {
            dfa_accepting[d] = 1;
        }

        /* 对每个符号计算转移 */
        for (int si = 0; si < 2; si++) {
            int next_subset = 0;

            /* 对 subset 中的每个 NFA 状态，计算 symbol 转移 */
            for (int s = 0; s < MAX_STATES; s++) {
                if (subset & (1 << s)) {
                    for (int t = 0; NFA_TRANS[s][si][t] != NO_TARGET; t++) {
                        int target = NFA_TRANS[s][si][t];
                        next_subset |= (1 << target);
                    }
                }
            }

            /* 计算ε-闭包 */
            next_subset = e_closure(next_subset);

            if (next_subset == 0) {
                /* 空集 — 死状态 */
                dfa_trans[d][si] = -1; /* 标记为无转移 */
            } else {
                /* 检查是否已存在 */
                int found = -1;
                for (int k = 0; k < dfa_count; k++) {
                    if (dfa_states[k] == next_subset) {
                        found = k;
                        break;
                    }
                }
                if (found == -1) {
                    /* 新 DFA 状态 */
                    dfa_states[dfa_count] = next_subset;
                    found = dfa_count;
                    dfa_count++;
                }
                dfa_trans[d][si] = found;
            }
        }
    }

    /* 打印 DFA 转换表 */
    printf("DFA Transition Table (from Subset Construction):\n");
    printf("%-10s %-20s %-8s %-8s %-8s\n", "DFA_State", "NFA_Subset", "a", "b", "Accept?");
    printf("%-10s %-20s %-8s %-8s %-8s\n", "----------", "--------------------", "--------", "--------", "--------");

    for (int d = 0; d < dfa_count; d++) {
        printf("D%-9d ", d);
        print_state_set(dfa_states[d], stdout);

        /* 对齐填充 */
        int printed = 0;
        int tmp = dfa_states[d];
        for (int s = 0; s < MAX_STATES; s++) {
            if (tmp & (1 << s)) printed++;
        }
        int padding = 20 - (2 + printed * 2); /* rough estimate */
        if (padding < 0) padding = 0;
        for (int p = 0; p < padding; p++) printf(" ");

        if (dfa_trans[d][0] >= 0)
            printf("D%-7d ", dfa_trans[d][0]);
        else
            printf("%-8s ", "-");

        if (dfa_trans[d][1] >= 0)
            printf("D%-7d ", dfa_trans[d][1]);
        else
            printf("%-8s ", "-");

        printf("%s\n", dfa_accepting[d] ? "Yes" : "No");
    }
    printf("\nTotal DFA states: %d\n", dfa_count);
}

int main(void) {
    printf("=== NFA: a*b | ab* ===\n");
    printf("States: 0-3, Alphabet: {a,b}, Start: 0, Accept: 3\n");
    printf("Epsilon transitions: 0->1, 0->2\n");
    printf("Transitions: 1-a->1, 1-b->3, 2-a->3, 3-b->3\n\n");

    /* ─── 1. ε-闭包计算 ─── */
    printf("=== Epsilon-Closures ===\n");
    for (int s = 0; s < MAX_STATES; s++) {
        int closure = e_closure(1 << s);
        printf("e-closure({%d}) = ", s);
        print_state_set(closure, stdout);
        printf("\n");
    }
    printf("\n");

    /* ─── 2. NFA 模拟 ─── */
    printf("=== NFA Simulation ===\n");
    NFA_simulate("aab", true);
    printf("\n");
    NFA_simulate("aba", true);
    printf("\n");

    /* ─── 3. 子集构造 ─── */
    printf("=== Subset Construction (NFA -> DFA) ===\n");
    subset_construct();

    return 0;
}
