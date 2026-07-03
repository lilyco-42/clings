/* 72_ansi-terminal-calc/terminal_calc.c — ANSI Terminal Calculator
 *
 * 任务：实现表达式求值计算器，用 ANSI 转义序列美化终端输出。
 *
 * 运算逻辑：
 *   - 内置固定表达式求值 (非交互式，避免终端依赖)
 *   - 2 个测试表达式："3+4*2" (=11) 和 "(5+3)*2" (=16)
 *   - 运算符优先级：先乘除 (* /) 后加减 (+ -)
 *   - 使用双栈算法：nums[] 存操作数，ops[] 存运算符
 *   - 括号通过将 '(' 压入 ops 栈，遇到 ')' 时弹栈直到 '(' 来处理
 *
 * 输出要求：
 *   - \033[32m 绿色显示正确结果
 *   - \033[31m 红色显示错误
 *   - \033[36m 青色显示解析头
 *   - \033[33m 黄色显示标题
 *   - \033[1m  加粗标题
 *   - \033[0m  重置样式
 *
 * 知识点：双栈表达式求值，运算符优先级，ANSI 转义序列，终端控制
 *
 * 验证：
 *   构建用 make；判分/自测用 clings run 或 clings watch
 *   clings 精确比对 stdout 原始字节 (含 ANSI 序列, clings tests 72 查看期望输出)
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─── ANSI 颜色宏 ───
 * 这些宏是字符串字面量，直接用在 printf 中即可。
 * 例如：printf(COLOR_GREEN "OK" COLOR_RESET "\n"); */
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"
#define COLOR_CYAN "\033[36m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RESET "\033[0m"

/* ─── TODO 1: parse_number ───
 * 从字符串 s 的位置 *pos 开始提取一个整数。
 * 用 while 循环累加 digit 字符：val = val*10 + (s[*pos] - '0')
 * 同时递增 *pos。
 * 返回解析出的整数值。
 *
 * 提示：使用 isdigit((unsigned char)s[*pos]) 判断是否为数字。
 *       多位数如 "42" 需要循环读取，不能只读一位。 */
static int parse_number(const char *s, int *pos) {
#error TODO 1: parse_number — while isdigit, accumulate val*10 + digit, advance *pos, return val
}

/* ─── TODO 2: precedence ───
 * 返回运算符 op 的优先级。
 * '*' 和 '/' 返回 2 (高优先级)。
 * '+' 和 '-' 返回 1 (低优先级)。
 * 其他字符返回 0。
 *
 * 提示：这是最简单的 TODO, 用 if 判断即可。 */
static int precedence(char op) {
#error TODO 2: precedence — return 2 for */ , 1 for +- , 0 otherwise
}

/* ─── TODO 3: apply_op ───
 * 对两个操作数 a, b 执行运算 op, 返回结果。
 * op 可能的值：'+', '-', '*', '/'。
 * 对于除法，检查 b != 0 防止除零错误 (除零时返回 0)。
 *
 * 提示：使用 switch(op) 结构，每个 case 返回对应运算结果。 */
static int apply_op(int a, int b, char op) {
#error TODO 3: apply_op — switch on op, return a+b, a-b, a*b, a/b (check b!=0)
}

/* ─── TODO 4: evaluate ───
 * 使用双栈算法求值表达式字符串 s。
 *
 * 双栈结构：
 *   int nums[64];   — 操作数栈 (最多 64 个数字)
 *   char ops[64];   — 运算符栈 (最多 64 个运算符)
 *   int ntop = 0;   — nums 栈顶索引
 *   int otop = 0;   — ops 栈顶索引
 *
 * 算法步骤：
 *   1. 用 while 循环遍历字符串 s 的每个字符 s[i]。
 *   2. 跳过空格：if (s[i] == ' ') { i++; continue; }
 *   3. 遇到数字：调用 parse_number(s, &i), 打印 "  parse_number: %d\n",
 *      将结果压入 nums[ntop++]。
 *   4. 遇到 '(' : 打印 "  enter paren: (\n", 将 '(' 压入 ops[otop++]。
 *   5. 遇到 ')' : 打印 "  exit paren: )\n",
 *      while (otop > 0 && ops[otop-1] != '('):
 *        弹出 b=nums[--ntop], a=nums[--ntop], op=ops[--otop],
 *        调用 apply_op(a,b,op), 打印 "  apply_op: %d %c %d = %d\n",
 *        结果压入 nums[ntop++]。
 *      弹出 '(' (otop--)。
 *   6. 遇到运算符 (+,-,*,/):
 *      计算当前运算符优先级 prec = precedence(s[i])。
 *      打印 "  operator: %c (precedence=%d)\n", s[i], prec。
 *      while (otop>0 && ops[otop-1]!='(' && precedence(ops[otop-1]) >= prec):
 *        弹出两个操作数和一个运算符，执行 apply_op, 打印结果，压回。
 *      将 s[i] 压入 ops[otop++], i++。
 *   7. 遍历结束后，while (otop > 0):
 *        弹出并执行剩余的运算符 (同步骤 5 中的弹栈计算)。
 *   8. 返回 nums[0] (栈底即最终结果)。
 *
 * 提示：先处理数字和括号，再处理运算符。注意 i 的位置管理：
 *       parse_number 会自动推进 i, 运算符分支手动 i++。 */
static int evaluate(const char *s) {
#error TODO 4: evaluate — two-stack algorithm: nums[64], ops[64], ntop/otop, loop over s, handle num/(/)/op, process remaining ops
}

/* ─── TODO 5: print_colored ───
 * 用 ANSI 颜色打印文本。
 *
 * 参数：
 *   color: ANSI 颜色码 (如 COLOR_GREEN "\033[32m")
 *   text:  要打印的文本 (纯文本，不含颜色)
 *
 * 实现：printf("%s%s" COLOR_RESET, color, text);
 *
 * 提示：打印完 text 后必须打印 COLOR_RESET 恢复终端默认样式。
 *       否则后续所有输出都会保持该颜色！*/
static void print_colored(const char *color, const char *text) {
#error TODO 5: print_colored — printf color, text, then COLOR_RESET
}

/* ─── TODO 6: main ───
 * 主函数：对两个测试表达式求值并打印彩色结果。
 *
 * 测试数据：
 *   const char *expressions[] = {"3+4*2", "(5+3)*2"};
 *   int expected[] = {11, 16};
 *   int num_expr = 2;
 *
 * 输出要求：
 *   1. 标题行 (加粗 + 黄色):
 *      printf(COLOR_BOLD COLOR_YELLOW "=== ANSI Terminal Calculator ===" COLOR_RESET "\n");
 *   2. 提示行：printf("Evaluating %d expressions with ANSI color output.\n\n", num_expr);
 *   3. for 循环处理每个表达式：
 *      a. 打印青色解析头：printf(COLOR_CYAN "Parsing: \"%s\"" COLOR_RESET "\n", expr);
 *      b. 调用 evaluate(expr) 得到结果。
 *      c. 打印 "  Result: %d\n", result。
 *      d. 比较 result == expected[i]:
 *         - 正确：printf(COLOR_GREEN "  OK: %s = %d" COLOR_RESET "\n\n", expr, result);
 *                  correct++。
 *         - 错误：printf(COLOR_RED "  FAIL: %s = %d (expected %d)" COLOR_RESET "\n\n", ...);
 *   4. 汇总 (加粗 + 黄色):
 *      printf(COLOR_BOLD COLOR_YELLOW "=== Summary ===" COLOR_RESET "\n");
 *      printf("Expressions evaluated: %d\n", num_expr);
 *      printf("Correct: " COLOR_GREEN "%d" COLOR_RESET "\n", correct);
 *      printf("Errors:  " COLOR_RED "%d" COLOR_RESET "\n", num_expr - correct);
 *   5. return 0。
 *
 * 提示：注意所有 ANSI 颜色后都要跟 COLOR_RESET, 否则颜色会"泄漏"到后续输出。
 *       每个表达式的输出后有一个空行 (printf("\n\n") 或等价)。 */
int main(void) {
#error TODO 6: main — define expressions[] and expected[], loop, call evaluate, print colored results, print summary
}
