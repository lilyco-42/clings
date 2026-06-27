/* 34_bracket_match.c — 栈实现括号匹配
 *
 * 任务: 1. 实现 is_match(open, close) — 判断左右括号是否配对（() [] {} 三种）
 *       2. 实现 check_brackets(s) — 用栈判断括号是否全部正确配对
 *
 * 栈的四个操作已提供: push / pop / is_empty（但 is_match 需要你自己写）
 *
 * 算法: 遍历字符串，左括号入栈，右括号与栈顶匹配
 *       栈空且遍历完 = yes，否则 = no
 *
 * 知识点: 栈 LIFO、括号配对映射、配对失败（不匹配+栈空+多余的左括号）
 *
 * 验证：
 *   stdin: "()\n"       → yes
 *   stdin: "()[]{}\n"   → yes
 *   stdin: "([)]\n"     → no   （交叉嵌套！）
 *   stdin: "(\n"        → no   （缺少右括号）
 *   stdin: ")\n"        → no   （缺少左括号）
 */
#include <stdio.h>
#define MAX 1024

int is_empty(int top) { return top == -1; }
void push(char *stack, int *top, char c) { stack[++(*top)] = c; }
char pop(char *stack, int *top) { return stack[(*top)--]; }

/* 判断 open 和 close 是否是一对匹配的括号
 * 支持 () [] {} 三种 */
int is_match(char open, char close) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 逐一检查三种配对：( 配 )、[ 配 ]、{ 配 } */

    /* 任意一种匹配返回 1，都不匹配返回 0 */
}

/* 用栈检测括号是否全部正确配对 */
int check_brackets(const char *s) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 创建栈（char 数组 + top 索引） */

    /* 遍历字符串 s 的每个字符：

     *   - 如果是左括号 ( [ { → 入栈

     *   - 如果是右括号 ) ] } → 检查：
     *        若栈空（右括号多了）→ 失败
     *        若栈顶不是对应的左括号 → 失败
     *        否则出栈（匹配成功） */

    /* 循环结束后：栈空 = 成功，栈非空 = 有多余的左括号 */
}

int main(void) {
    char s[MAX];
    fgets(s, sizeof(s), stdin);
    int i = 0;
    while (s[i] && s[i] != '\n') i++;
    s[i] = '\0';
    printf("%s\n", check_brackets(s) ? "yes" : "no");
    return 0;
}
