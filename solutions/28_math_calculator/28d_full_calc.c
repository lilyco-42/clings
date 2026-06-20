/*
 * Lesson 28d: 完整计算器 — 参考答案
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

static double parse_factor(void)
{
    skip_spaces();

    /* 一元负号 */
    if (*pos == '-') {
        pos++;
        return -parse_factor();
    }

    /* 括号 */
    if (*pos == '(') {
        pos++;
        double val = parse_expr();
        skip_spaces();
        if (*pos == ')') pos++;
        return val;
    }

    /* 数学函数 */
    if (isalpha(*pos)) {
        char func[16] = {0};
        int i = 0;
        while (isalpha(*pos) && i < 15)
            func[i++] = *pos++;
        skip_spaces();
        if (*pos == '(') pos++;
        double arg = parse_expr();
        skip_spaces();
        if (*pos == ')') pos++;

        if (strcmp(func, "sin") == 0)  return sin(arg);
        if (strcmp(func, "cos") == 0)  return cos(arg);
        if (strcmp(func, "tan") == 0)  return tan(arg);
        if (strcmp(func, "sqrt") == 0) return sqrt(arg);
        if (strcmp(func, "abs") == 0)  return fabs(arg);
        if (strcmp(func, "log") == 0)  return log(arg);
        if (strcmp(func, "exp") == 0)  return exp(arg);
        fprintf(stderr, "Error: unknown function '%s'\n", func);
        return 0;
    }

    return parse_number();
}

static double parse_term(void)
{
    double val = parse_factor();
    skip_spaces();
    while (*pos == '*' || *pos == '/') {
        char op = *pos++;
        double right = parse_factor();
        if (op == '*') val *= right;
        else {
            if (right == 0) {
                fprintf(stderr, "Error: division by zero\n");
                exit(1);
            }
            val /= right;
        }
        skip_spaces();
    }
    return val;
}

static double parse_expr(void)
{
    double val = parse_term();
    skip_spaces();
    while (*pos == '+' || *pos == '-') {
        char op = *pos++;
        double right = parse_term();
        if (op == '+') val += right;
        else val -= right;
        skip_spaces();
    }
    return val;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s expression\n", argv[0]);
        return 1;
    }

    char expr[1024] = {0};
    for (int i = 1; i < argc; i++) {
        if (i > 1) strcat(expr, " ");
        strncat(expr, argv[i], sizeof(expr) - strlen(expr) - 1);
    }

    pos = expr;
    double result = parse_expr();

    if (result == (long)result && result > -1e15 && result < 1e15)
        printf("%ld\n", (long)result);
    else
        printf("%.6g\n", result);

    return 0;
}
