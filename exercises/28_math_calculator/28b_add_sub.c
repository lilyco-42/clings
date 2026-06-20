/*
 * Lesson 28b: 加减法解析器
 *
 * 知识点：递归下降入门, 最简单的 expr = number (('+' | '-') number)*
 *
 * 任务：
 *   实现只支持加减法的表达式计算器。
 *   此时 expr 和 term 是同一层（没有优先级差异）。
 *
 *   文法: expr = number (('+' | '-') number)*
 *
 *   示例：
 *     ./28b_add_sub "2+3"      → "5\n"
 *     ./28b_add_sub "10-3+2"   → "9\n"
 *     ./28b_add_sub "100"      → "100\n"
 */

#include <stdio.h>
#include <stdlib.h>

static const char *pos;

static void skip_spaces(void)
{
    while (*pos == ' ' || *pos == '\t')
        pos++;
}

static double parse_number(void)
{
    skip_spaces();
    char *end;
    double val = strtod(pos, &end);
    if (end == pos) {
        fprintf(stderr, "Error: expected number at '%s'\n", pos);
        exit(1);
    }
    pos = end;
    return val;
}

#error TODO: Fix this exercise. Run "clings hint" for help.
/* 实现 parse_expr:
 * static double parse_expr(void)
 * {
 *     double val = parse_number();
 *     skip_spaces();
 *     while (*pos == '+' || *pos == '-') {
 *         char op = *pos++;
 *         double right = parse_number();
 *         if (op == '+') val += right;
 *         else val -= right;
 *         skip_spaces();
 *     }
 *     return val;
 * }
 */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s expression\n", argv[0]);
        return 1;
    }

    pos = argv[1];
    /* 调用 parse_expr, 打印结果 */
    return 0;
}
