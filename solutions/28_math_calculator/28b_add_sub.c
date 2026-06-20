/*
 * Lesson 28b: 加减法解析器 — 参考答案
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

static double parse_expr(void)
{
    double val = parse_number();
    skip_spaces();
    while (*pos == '+' || *pos == '-') {
        char op = *pos++;
        double right = parse_number();
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

    pos = argv[1];
    double result = parse_expr();

    if (result == (long)result && result > -1e15 && result < 1e15)
        printf("%ld\n", (long)result);
    else
        printf("%.6g\n", result);

    return 0;
}
