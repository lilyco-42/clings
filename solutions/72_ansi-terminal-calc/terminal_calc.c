/* 72_ansi-terminal-calc/terminal_calc.c — ANSI Terminal Calculator
 *
 * Task: Implement a simple expression calculator with ANSI color output.
 *       Use the two-stack algorithm: parse numbers and operators,
 *       first evaluate * and / (high precedence), then + and -.
 *
 * Expressions (hardcoded): "3+4*2" (=11), "(5+3)*2" (=16)
 *
 * Implementation:
 *   1. parse_number: extract integer from string at position
 *   2. precedence: return precedence level of an operator
 *   3. apply_op: apply operator to two operands
 *   4. evaluate: two-stack expression evaluator
 *   5. print_colored: print text with ANSI color
 *   6. main: iterate over expressions, compare with expected
 *
 * Key concepts: two-stack evaluation, operator precedence, ANSI escape sequences,
 *                terminal colors, integer arithmetic
 *
 * Verification: make test compares against expected_output.txt
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ─── ANSI Color Macros ─── */
#define COLOR_GREEN "\033[32m"
#define COLOR_RED "\033[31m"
#define COLOR_CYAN "\033[36m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RESET "\033[0m"

/* ─── parse_number ───
 * Extract an integer from s starting at position *pos.
 * Advances *pos past the digits read.
 * Returns the parsed integer value. */
static int parse_number(const char *s, int *pos) {
    int val = 0;
    while (isdigit((unsigned char)s[*pos])) {
        val = val * 10 + (s[*pos] - '0');
        (*pos)++;
    }
    return val;
}

/* ─── precedence ───
 * Return precedence of operator: 2 for * and /, 1 for + and -, 0 otherwise. */
static int precedence(char op) {
    if (op == '*' || op == '/') return 2;
    if (op == '+' || op == '-') return 1;
    return 0;
}

/* ─── apply_op ───
 * Apply operator op to two integers a and b.
 * Returns the result of a op b. */
static int apply_op(int a, int b, char op) {
    switch (op) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return b != 0 ? a / b : 0;
        default:
            return 0;
    }
}

/* ─── evaluate ───
 * Evaluate an arithmetic expression string.
 * Uses two stacks: nums[] for values, ops[] for operators.
 * Handles +, -, *, / with correct precedence and ( ) parentheses.
 * Also prints step-by-step parsing information.
 * Returns the computed integer result. */
static int evaluate(const char *s) {
    int nums[64];
    char ops[64];
    int ntop = 0, otop = 0;
    int i = 0, len = (int)strlen(s);

    while (i < len) {
        /* Skip whitespace */
        if (s[i] == ' ') {
            i++;
            continue;
        }

        /* Number */
        if (isdigit((unsigned char)s[i])) {
            int val = parse_number(s, &i);
            printf("  parse_number: %d\n", val);
            nums[ntop++] = val;
            continue;
        }

        /* Opening parenthesis */
        if (s[i] == '(') {
            printf("  enter paren: (\n");
            ops[otop++] = '(';
            i++;
            continue;
        }

        /* Closing parenthesis */
        if (s[i] == ')') {
            printf("  exit paren: )\n");
            while (otop > 0 && ops[otop - 1] != '(') {
                int b = nums[--ntop];
                int a = nums[--ntop];
                char op = ops[--otop];
                int res = apply_op(a, b, op);
                printf("  apply_op: %d %c %d = %d\n", a, op, b, res);
                nums[ntop++] = res;
            }
            if (otop > 0) otop--; /* pop '(' */
            i++;
            continue;
        }

        /* Operator */
        if (s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/') {
            int prec = precedence(s[i]);
            printf("  operator: %c (precedence=%d)\n", s[i], prec);
            while (otop > 0 && ops[otop - 1] != '(' && precedence(ops[otop - 1]) >= prec) {
                int b = nums[--ntop];
                int a = nums[--ntop];
                char op = ops[--otop];
                int res = apply_op(a, b, op);
                printf("  apply_op: %d %c %d = %d\n", a, op, b, res);
                nums[ntop++] = res;
            }
            ops[otop++] = s[i];
            i++;
            continue;
        }

        i++; /* skip unknown chars */
    }

    /* Process remaining operators */
    while (otop > 0) {
        int b = nums[--ntop];
        int a = nums[--ntop];
        char op = ops[--otop];
        int res = apply_op(a, b, op);
        printf("  apply_op: %d %c %d = %d\n", a, op, b, res);
        nums[ntop++] = res;
    }

    return nums[0];
}

/* ─── print_colored ───
 * Print text surrounded by ANSI color escape sequences.
 * color: the ANSI color code string (e.g. COLOR_GREEN).
 * text:  the text to print.
 * Returns void. */
static void print_colored(const char *color, const char *text) { printf("%s%s" COLOR_RESET, color, text); }

/* ─── main ───
 * Evaluate two hardcoded test expressions and print colored results.
 * Expression 1: "3+4*2" expected 11
 * Expression 2: "(5+3)*2" expected 16
 * Prints title, steps, results with ANSI colors. */
int main(void) {
    const char *expressions[] = {"3+4*2", "(5+3)*2"};
    int expected[] = {11, 16};
    int num_expr = 2;
    int correct = 0;

    /* Title — demonstrates print_colored */
    print_colored(COLOR_BOLD COLOR_YELLOW, "=== ANSI Terminal Calculator ===");
    printf("\n");
    printf("Evaluating %d expressions with ANSI color output.\n\n", num_expr);

    /* Evaluate each expression */
    for (int i = 0; i < num_expr; i++) {
        printf(COLOR_CYAN "Parsing: \"%s\"" COLOR_RESET "\n", expressions[i]);
        int result = evaluate(expressions[i]);
        printf("  Result: %d\n", result);

        if (result == expected[i]) {
            printf(COLOR_GREEN "  OK: %s = %d" COLOR_RESET "\n\n", expressions[i], result);
            correct++;
        } else {
            printf(COLOR_RED "  FAIL: %s = %d (expected %d)" COLOR_RESET "\n\n", expressions[i], result, expected[i]);
        }
    }

    /* Summary */
    printf(COLOR_BOLD COLOR_YELLOW "=== Summary ===" COLOR_RESET "\n");
    printf("Expressions evaluated: %d\n", num_expr);
    printf("Correct: " COLOR_GREEN "%d" COLOR_RESET "\n", correct);
    printf("Errors:  " COLOR_RED "%d" COLOR_RESET "\n", num_expr - correct);

    return 0;
}
