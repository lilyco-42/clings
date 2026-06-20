/*
 * Lesson 28d: 完整计算器（括号 + 一元负号 + 数学函数）
 *
 * 知识点：递归调用处理括号, 一元运算符, 函数名识别
 *
 * 任务：
 *   在 28c 的基础上，引入 factor 层处理括号、一元负号和数学函数：
 *
 *   文法:
 *     expr   = term (('+' | '-') term)*
 *     term   = factor (('*' | '/') factor)*
 *     factor = '-' factor | '(' expr ')' | number | func '(' expr ')'
 *
 *   支持的数学函数: sin, cos, tan, sqrt, abs, log, exp
 *
 *   示例：
 *     ./28d_full_calc "(1+2)*(3+4)"    → "21\n"
 *     ./28d_full_calc "-(-5)"          → "5\n"
 *     ./28d_full_calc "sqrt(16)"       → "4\n"
 *     ./28d_full_calc "2+3*4"          → "14\n"
 *
 *   注意: 链接时需要 -lm (math library)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

static const char *pos;

static double parse_expr(void);

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
/* 实现 parse_factor:
 *
 * static double parse_factor(void)
 * {
 *     skip_spaces();
 *     if (*pos == '-') { pos++; return -parse_factor(); }        // 一元负号
 *     if (*pos == '(') { pos++; double v = parse_expr(); skip_spaces(); if (*pos == ')') pos++; return v; }
 *     if (isalpha(*pos)) {                                       // 函数名
 *         char func[16] = {0}; int i = 0;
 *         while (isalpha(*pos) && i < 15) func[i++] = *pos++;
 *         skip_spaces(); if (*pos == '(') pos++;
 *         double arg = parse_expr();
 *         skip_spaces(); if (*pos == ')') pos++;
 *         if (strcmp(func,"sin")==0) return sin(arg);
 *         if (strcmp(func,"cos")==0) return cos(arg);
 *         if (strcmp(func,"sqrt")==0) return sqrt(arg);
 *         ...
 *     }
 *     return parse_number();
 * }
 *
 * 然后实现 parse_term 和 parse_expr（同 28c）。
 */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s expression\n", argv[0]);
        return 1;
    }

    /* 拼接所有参数 */
    char expr[1024] = {0};
    for (int i = 1; i < argc; i++) {
        if (i > 1) strcat(expr, " ");
        strncat(expr, argv[i], sizeof(expr) - strlen(expr) - 1);
    }

    pos = expr;
    /* 调用 parse_expr, 打印结果 */
    return 0;
}
