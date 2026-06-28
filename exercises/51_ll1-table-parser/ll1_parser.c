/* 51_ll1-table-parser.c — LL(1) 预测分析表构造与语法解析
 *
 * 任务：1. 实现 token_id()  — 词法符号到枚举的映射
 *       2. 实现栈操作 push/pop/empty/stack_str
 *       3. 实现 compute_first()  — 位掩码不动点迭代计算 FIRST 集
 *       4. 实现 compute_follow() — 位掩码不动点迭代计算 FOLLOW 集
 *       5. 实现 build_table()   — 由 FIRST/FOLLOW 构建预测分析表
 *       6. 补全 main() 中的 LL(1) 解析主循环
 *
 * 背景：LL(1) 是自顶向下语法分析的经典算法。"LL" = 从左到右扫描、
 *       最左推导；"(1)" = 每次只看 1 个前瞻符号。预测分析表由
 *       FIRST/FOLLOW 集自动构造，是编译器前端核心组件。
 *
 * 文法：E→TE', E'→+TE'|ε, T→FT', T'→*FT'|ε, F→(E)|id
 *
 * 知识点：FIRST/FOLLOW 集、位掩码迭代、预测分析表构造、栈式解析
 *
 * 验证：
 *   make test — 编译并运行，diff 比对 expected_output.txt
 */
#include <stdio.h>
#include <string.h>

#define STACK_SZ 64
#define PROD_CNT 8

/* ─── 符号定义 ─── */
/* 终结符（词法单元） */
enum { T_ID, T_PLUS, T_STAR, T_LPAR, T_RPAR, T_EOF, T_COUNT = 6 };
/* 非终结符（语法变量） */
enum { NT_E, NT_Ee, NT_T, NT_Te, NT_F, NT_COUNT = 5 };
/* 符号类型 */
enum { SYM_TERM, SYM_NONTERM };

typedef struct {
    int type;
    int id;
} Sym; /* 一个栈元素 */

/* 名称映射 */
const char *tname[T_COUNT] = {"id", "+", "*", "(", ")", "$"};
const char *nname[NT_COUNT] = {"E", "E'", "T", "T'", "F"};

/* 产生式显示字符串 */
const char *prod_label[PROD_CNT] = {
    "E  → T E'",   /* 0 */
    "E' → + T E'", /* 1 */
    "E' → ε",      /* 2 */
    "T  → F T'",   /* 3 */
    "T' → ε",      /* 4 */
    "T' → * F T'", /* 5 */
    "F  → id",     /* 6 */
    "F  → ( E )",  /* 7 */
};

/* RHS 展开序列：终结符用枚举值 (0-5), 非终结符 = id+100, -1=结束
 *
 * 注意：rhs[p][0] 存储的是最右侧符号，rhs[p][last] 是最左侧符号。
 * 这是为了方便栈压入（LIFO：先压入右侧符号）。
 */
#define NT(x) ((x) + 100)
int rhs[PROD_CNT][4] = {
    {NT(NT_Ee), NT(NT_T), -1},         /* 0: E  → T E'      */
    {NT(NT_Ee), NT(NT_T), T_PLUS, -1}, /* 1: E' → + T E'    */
    {-1},                              /* 2: E' → ε          */
    {NT(NT_Te), NT(NT_F), -1},         /* 3: T  → F T'      */
    {-1},                              /* 4: T' → ε          */
    {NT(NT_Te), NT(NT_F), T_STAR, -1}, /* 5: T' → * F T'    */
    {T_ID, -1},                        /* 6: F  → id         */
    {T_RPAR, NT(NT_E), T_LPAR, -1},    /* 7: F  → ( E )      */
};

/* 产生式 → LHS 非终结符映射 */
int prod_lhs[PROD_CNT] = {NT_E, NT_Ee, NT_Ee, NT_T, NT_Te, NT_Te, NT_F, NT_F};

/* ─── FIRST / FOLLOW / 预测分析表（由学生填充） ─── */
int first[NT_COUNT];          /* 位掩码：bit t = 1 表示终结符 t ∈ FIRST(NT) */
int follow[NT_COUNT];         /* 位掩码：bit t = 1 表示终结符 t ∈ FOLLOW(NT) */
int table[NT_COUNT][T_COUNT]; /* table[NT][T] = 产生式编号，-1 = 语法错误   */

/* ─── 位掩码辅助宏 ─── */
#define HAS(bitmask, t) ((bitmask) & (1 << (t)))
#define ADD(bitmask, t) ((bitmask) |= (1 << (t)))

/* ─── 工具函数（已提供） ─── */

/* 获取 rhs 数组长度（-1 之前元素个数） */
static int rhs_len(int p) {
    int len = 0;
    while (rhs[p][len] != -1) len++;
    return len;
}

/* 获取产生式 p 从左到右的第 i 个符号（0 = 第一个符号）
 * rhs 是逆序存储的，所以索引映射为：正向索引 i → rhs[p][len-1-i] */
static int rhs_at(int p, int i) {
    int len = rhs_len(p);
    return rhs[p][len - 1 - i];
}

/* 判断符号是否为终结符 */
static int is_terminal(int sym_id) { return sym_id >= 0 && sym_id < T_COUNT; }

/* 输入序列 → 字符串 */
static void input_str(int *tokens, int pos, int n, char *buf) {
    int p = 0;
    for (int i = pos; i < n; i++) {
        const char *s = tname[tokens[i]];
        while (*s) buf[p++] = *s++;
    }
    buf[p] = '\0';
}

/* 位掩码 → 终结符字符串（用于打印 FIRST/FOLLOW 集） */
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

/* ─── TODO 1: token_id ────────────────────────────────────────
 *
 * 将词法字符串映射为终结符枚举值。
 *
 * 参数：s — 词法字符串 ("id", "+", "*", "(", ")", "$")
 * 返回：对应的枚举值 (0~5)，未找到返回 -1
 *
 * 提示：用 for 循环遍历 tname[] 数组，strcmp 比较。
 */
static int token_id(const char *s) {
#error TODO 1: token_id — map token string to terminal enum (0~5)
}

/* ─── TODO 2: 栈操作 ──────────────────────────────────────────
 *
 * 实现 push, pop, empty, stack_str 四个栈操作函数。
 *
 * 栈定义（已提供）：
 *   Sym  stack[STACK_SZ];   // 栈数组
 *   int  top_idx = -1;      // 栈顶索引，-1 表示空栈
 *
 * push(type, id): 创建 Sym 结构体 {type, id}，存入 stack[++top_idx]
 * pop():          返回 stack[top_idx--]
 * empty():        返回 top_idx < 0
 * stack_str(buf): 遍历 stack[0..top_idx]，将每个符号的名称拼接到 buf
 *                 根据 type 选择 tname[id] 或 nname[id]
 */
Sym stack[STACK_SZ];
int top_idx = -1;

#error TODO 2: push, pop, empty, stack_str — implement stack operations
static void push(int type, int id) { /* TODO: 创建 Sym 结构体，top_idx++ 压入 */ }

static Sym pop(void) { /* TODO: top_idx-- 并返回栈顶元素 */ }

static int empty(void) { /* TODO: top_idx < 0 时返回真 */ }

static void stack_str(char *buf) { /* TODO: 将栈中所有符号的名称拼接写入 buf */ }

/* ─── TODO 3: compute_first ───────────────────────────────────
 *
 * 用不动点迭代计算所有非终结符的 FIRST 集，结果存入 first[] 位掩码。
 *
 * FIRST 集定义：FIRST(X) = { a | X ⇒* a... , a 是终结符 }
 *
 * 不动点迭代规则（对每个产生式 A → X1 X2 ... Xn）：
 *   for i = 1 to n:
 *     FIRST(A) ⊇ FIRST(Xi) - {ε}    （Xi 的 FIRST 中所有非 ε 终结符加入 A）
 *     若 Xi 不可空（即 Xi 不能推导出 ε），则停止
 *   （本练习中 E' 和 T' 可空，因为它们有 ε 产生式）
 *
 * 实现要点：
 *   - 初始化 first[] 全为 0
 *   - 使用 do-while 循环，changed 标记是否发生变化
 *   - 对每个产生式 p，获取 LHS = prod_lhs[p]
 *   - 按从左到右顺序遍历 RHS 符号（使用 rhs_at(p, i)）
 *   - 终结符：直接 ADD 到 first[A]，停止
 *   - 非终结符 X：将 first[X-100] 的所有位复制到 first[A]，若 X 不可空则停止
 *   - 使用 ADD/HAS 宏操作位掩码
 */
#error TODO 3: compute_first — fixed-point iteration with bitmasks
static void compute_first(void) { /* TODO: 不动点迭代计算 FIRST 集 */ }

/* ─── TODO 4: compute_follow ──────────────────────────────────
 *
 * 用不动点迭代计算所有非终结符的 FOLLOW 集，结果存入 follow[] 位掩码。
 *
 * FOLLOW 集定义：FOLLOW(B) = { a | S ⇒* ...B a... , a 是终结符 }
 *
 * 不动点迭代规则（对每个产生式 A → αBβ）：
 *   规则 1：FOLLOW(B) ⊇ FIRST(β) - {ε}
 *   规则 2：若 β ⇒* ε（或 β 不存在，即 A → αB），则 FOLLOW(B) ⊇ FOLLOW(A)
 *
 * 实现要点：
 *   - 初始化 follow[] 全为 0
 *   - ADD(follow[NT_E], T_EOF) — $ ∈ FOLLOW(开始符号 E)
 *   - 使用 do-while 循环，changed 标记
 *   - 对每个产生式 p，遍历 RHS 中每个非终结符 B（位置 i）
 *   - 对 B 之后的每个符号 Y（位置 j = i+1 起）：
 *       若 Y 是终结符：ADD(follow[B], Y)，停止（β 不可全空）
 *       若 Y 是非终结符：将 first[Y-100] 复制到 follow[B]
 *         若 Y 不可空，停止（β 不可全空）
 *   - 若 β 全可空（遍历完所有 j 都未因不可空而停止）：将 follow[A] 复制到 follow[B]
 */
#error TODO 4: compute_follow — fixed-point iteration with bitmasks
static void compute_follow(void) { /* TODO: 不动点迭代计算 FOLLOW 集 */ }

/* ─── TODO 5: build_table ─────────────────────────────────────
 *
 * 由 FIRST/FOLLOW 集构建预测分析表 table[NT][T]。
 *
 * 构建规则（对每个产生式 p: A → α）：
 *   1. 初始化 table[][] 全为 -1
 *   2. 计算 FIRST(α)：
 *       对 α 中从左到右每个符号 Xi：
 *         若 Xi 是终结符 a：table[A][a] = p，停止
 *         若 Xi 是非终结符：对每个 t ∈ FIRST(Xi)，table[A][t] = p
 *           若 Xi 不可空，停止
 *   3. 若 α ⇒* ε（即 α 中所有符号都可空，或 α = ε）：
 *       对每个 b ∈ FOLLOW(A)，table[A][b] = p
 *
 * 实现要点：
 *   - 先双重循环将 table[nt][t] 全设为 -1
 *   - 对每个产生式 p：判断 RHS 是否为空（len == 0）
 *     - 空 RHS（ε 产生式）：对每个 b ∈ FOLLOW(A)，table[A][b] = p
 *     - 非空 RHS：遍历符号，终结符直接填表停止，非终结符复制 FIRST 集
 *       若所有符号都可空（nullable_prod），再对 FOLLOW(A) 填表
 */
#error TODO 5: build_table — construct predictive parsing table from FIRST/FOLLOW
static void build_table(void) { /* TODO: 由 FIRST/FOLLOW 集构建预测分析表 */ }

/* ─── TODO 6: 打印 FIRST/FOLLOW 集 ────────────────────────────
 *
 * 按照 expected_output.txt 的格式打印 FIRST 和 FOLLOW 集。
 *
 * 输出格式：
 *   === FIRST Sets ===
 *   FIRST(E) = {id, (}
 *   FIRST(E') = {+}
 *   ...
 *   (空行)
 *   === FOLLOW Sets ===
 *   FOLLOW(E) = {), $}
 *   ...
 *   (空行)
 *
 * 提示：使用 set_str() 函数将位掩码转为字符串，然后 printf。
 */
#error TODO 6: print_sets — print FIRST and FOLLOW sets
static void print_sets(void) { /* TODO: 打印 FIRST 和 FOLLOW 集 */ }

/* ─── 主解析循环 ─── */
int main(void) {
    /* 预定义输入：id + id * id */
    char *input[] = {"id", "+", "id", "*", "id", "$", NULL};
    int tokens[16], n = 0;
    for (int i = 0; input[i]; i++) tokens[n++] = token_id(input[i]);

    /* 计算 FIRST / FOLLOW / 预测分析表 */
    compute_first();
    compute_follow();
    build_table();

    printf("=== LL(1) Table-Driven Parser ===\n");
    printf("Grammar: E→TE' E'→+TE'|ε T→FT' T'→*FT'|ε F→id|(E)\n");
    printf("Input:   id + id * id\n\n");

    print_sets();

    /* 初始化：压入 $ 和开始符号 E */
    push(SYM_TERM, T_EOF);
    push(SYM_NONTERM, NT_E);

#error TODO 7: LL(1) parsing main loop — table-driven parse
    /* ══════════════════════════════════════════════
     * LL(1) 解析主循环
     * ══════════════════════════════════════════════
     *
     * int ip = 0;    // 输入指针
     * int step = 0;  // 步骤编号
     * char s_buf[64], i_buf[64];
     *
     * while (!empty()):
     *   ① 获取栈顶 s_top = stack[top_idx]（不 pop）
     *   ② 获取当前输入符号 cur = tokens[ip]
     *   ③ 打印当前步骤：
     *      stack_str(s_buf); input_str(tokens, ip, n, i_buf);
     *      printf("[%02d] %-18s %-16s ", step, s_buf, i_buf);
     *
     *   ④ 若 s_top.type == SYM_TERM（终结符）：
     *       若 s_top.id == cur: printf("match %s\n", tname[cur]); pop(); ip++;
     *       否则：printf("ERROR\n"); return 1;
     *
     *   ⑤ 若 s_top.type == SYM_NONTERM（非终结符）：
     *       查表 pid = table[s_top.id][cur]
     *       pid < 0 → printf("ERROR\n"); return 1;
     *       否则：printf("%s\n", prod_label[pid]); pop();
     *       遍历 rhs[pid]（从 i=0 到 -1）：
     *         若 rhs[pid][i] >= 100：push(SYM_NONTERM, rhs[pid][i] - 100)
     *         否则：push(SYM_TERM, rhs[pid][i])
     *
     *   ⑥ step++
     *
     * 循环结束后，打印最终状态 + ACCEPT：
     *   stack_str(s_buf); input_str(tokens, ip, n, i_buf);
     *   printf("[%02d] %-18s %-16s ACCEPT\n", step, s_buf, i_buf);
     * ══════════════════════════════════════════════ */
    return 0;
}
