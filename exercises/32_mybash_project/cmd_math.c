/*
 * cmd_math.c - Expression calculator with operator precedence
 *
 * Lesson 28: Recursive descent parser for arithmetic expressions.
 * Supports: +, -, *, /, (), unary minus, and math functions.
 *
 * Grammar:
 *   expr   = term (('+' | '-') term)*
 *   term   = factor (('*' | '/') factor)*
 *   factor = '-' factor | '(' expr ')' | number | func '(' expr ')'
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

static const char *pos; /* current parse position */

static double parse_expr(void);

static void skip_spaces(void)
{
    while (*pos == ' ' || *pos == '\t') pos++;
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

    /* unary minus */
    if (*pos == '-') {
        pos++;
        return -parse_factor();
    }

    /* parenthesized expression */
    if (*pos == '(') {
        pos++;
        double val = parse_expr();
        skip_spaces();
        if (*pos == ')') pos++;
        return val;
    }

    /* math functions: sin, cos, sqrt, abs, log */
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
            if (right == 0) { fprintf(stderr, "Error: division by zero\n"); exit(1); }
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

int math_main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: math <expression>\n");
        printf("Examples: math \"2+3*4\"  math \"sqrt(16)\"  math \"(1+2)*(3+4)\"\n");
        return -1;
    }

    /* concatenate all arguments into one expression string */
    char expr[1024] = {0};
    for (int i = 1; i < argc; i++) {
        if (i > 1) strcat(expr, " ");
        strncat(expr, argv[i], sizeof(expr) - strlen(expr) - 1);
    }

    pos = expr;
    double result = parse_expr();

    /* print as integer if no decimal part */
    if (result == (int)result)
        printf("%d\n", (int)result);
    else
        printf("%.6g\n", result);

    return 0;
}
