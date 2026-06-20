/*
 * Lesson 28c: 乘除法与运算符优先级
 *
 * 知识点：引入 term 层实现优先级, 递归下降的层次结构
 *
 * 任务：
 *   在 28b 的基础上，正确处理乘除法优先级：
 *
 *   文法:
 *     expr = term (('+' | '-') term)*
 *     term = number (('*' | '/') number)*
 *
 *   关键: * / 在 term 层处理（优先级高），+ - 在 expr 层处理（优先级低）
 *
 *   示例：
 *     ./28c_mul_div "2+3*4"    → "14\n"   (不是 20!)
 *     ./28c_mul_div "10/2-1"   → "4\n"
 *     ./28c_mul_div "6/3/2"    → "1\n"    (左结合)
 *
 *   除零检测: 除数为 0 时打印 "Error: division by zero\n" 到 stderr 并 exit(1)
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
/* 实现 parse_term 和 parse_expr:
 *
 * static double parse_term(void)
 * {
 *     double val = parse_number();
 *     skip_spaces();
 *     while (*pos == '*' || *pos == '/') {
 *         char op = *pos++;
 *         double right = parse_number();
 *         if (op == '*') val *= right;
 *         else {
 *             if (right == 0) { fprintf(stderr, "Error: division by zero\n"); exit(1); }
 *             val /= right;
 *         }
 *         skip_spaces();
 *     }
 *     return val;
 * }
 *
 * static double parse_expr(void)
 * {
 *     double val = parse_term();  // 注意: 这里调用 parse_term 而非 parse_number
 *     skip_spaces();
 *     while (*pos == '+' || *pos == '-') { ... }
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
