/*
 * Lesson 28a: 解析数字 — 参考答案
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

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s expression\n", argv[0]);
        return 1;
    }

    pos = argv[1];
    double val = parse_number();

    if (val == (long)val && val > -1e15 && val < 1e15)
        printf("%ld\n", (long)val);
    else
        printf("%.6g\n", val);

    return 0;
}
