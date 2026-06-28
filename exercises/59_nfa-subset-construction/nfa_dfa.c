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
 * 知识点：ε-闭包、NFA 模拟、子集构造 (幂集)、DFA 最小化
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

#define NO_TARGET -2
#define EPSILON -1

/* NFA 转移：trans[state][symbol] = 可达状态列表，以 NO_TARGET 结束 */
static const int NFA_TRANS[4][2][4] = {
    /* state 0 */ {{NO_TARGET}, {NO_TARGET}},
    /* state 1 */ {{1, NO_TARGET}, {3, NO_TARGET}},
    /* state 2 */ {{3, NO_TARGET}, {NO_TARGET}},
    /* state 3 */ {{NO_TARGET}, {3, NO_TARGET}},
};

/* ε转移：epsilon[state] = {可达状态列表，NO_TARGET 结束} */
static const int EPSILON_TRANS[4][4] = {
    {1, 2, NO_TARGET},
    {NO_TARGET},
    {NO_TARGET},
    {NO_TARGET},
};

/* ─── ε-闭包计算 ───
 * 给定状态集 (set, 用位掩码表示), 返回包含所有ε可达状态的位掩码
 * 参数：states — 位掩码，第 s 位为 1 表示状态 s 在集合中
 * 返回：闭包位掩码 */
static int e_closure(int states) {
#error TODO 1: 实现ε-闭包计算。重复扫描: 对闭包中每个状态, 若其ε目标不在闭包中则加入, 直至不动点。返回最终位掩码。
}

/* ─── 打印状态集 (已提供) ─── */
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
#error TODO 2: 实现NFA模拟。初态=ε-closure({0}); 逐字符读入, 对当前状态集中每个状态查NFA_TRANS求symbol转移目标, 再求ε-closure; 打印每步; 最终检查是否包含接受态3。
}

/* ─── 子集构造 NFA→DFA ───
 * 从 NFA 构造等价的 DFA, 打印 DFA 转换表 */
static void subset_construct(void) {
#error TODO 3: 实现子集构造。DFA初态=ε-closure({0}); 对每个未处理的DFA状态(子集), 对a和b分别计算转移后的子集(先symbol转移再ε-closure); 若子集已存在则复用, 否则新建DFA状态; 标记接受态(包含状态3); 打印表格。
}

int main(void) {
    printf("=== NFA: a*b | ab* ===\n");
    printf("States: 0-3, Alphabet: {a,b}, Start: 0, Accept: 3\n");
    printf("Epsilon transitions: 0->1, 0->2\n");
    printf("Transitions: 1-a->1, 1-b->3, 2-a->3, 3-b->3\n\n");

    /* ─── 1. ε-闭包计算 ─── */
    printf("=== Epsilon-Closures ===\n");
#error TODO 4: 对状态0~3分别调用e_closure并打印 "e-closure({s}) = {x,y,z}"
    printf("\n");

    /* ─── 2. NFA 模拟 ─── */
    printf("=== NFA Simulation ===\n");
#error TODO 5: 调用 NFA_simulate("aab", true) 和 NFA_simulate("aba", true)
    printf("\n");

    /* ─── 3. 子集构造 ─── */
    printf("=== Subset Construction (NFA -> DFA) ===\n");
#error TODO 6: 调用 subset_construct()

    return 0;
}
