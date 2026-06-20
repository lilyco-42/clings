/*
 * Lesson 35d: 中缀表达式转后缀（Shunting Yard）— 参考答案
 */
#include <stdio.h>
#include <ctype.h>

#define MAX_SIZE 256

char stack[MAX_SIZE];
int top = -1;

void push(char c) { stack[++top] = c; }
char pop(void) { return stack[top--]; }
char peek(void) { return stack[top]; }
int is_empty(void) { return top == -1; }

int priority(char op)
{
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

void infix_to_postfix(const char *expr, char *output)
{
    int j = 0;
    while (*expr) {
        if (isdigit((unsigned char)*expr)) {
            output[j++] = *expr;
        } else if (*expr == '(') {
            push(*expr);
        } else if (*expr == ')') {
            while (!is_empty() && peek() != '(')
                output[j++] = pop();
            pop();  /* 弹出 '(' */
        } else {
            /* 运算符 */
            while (!is_empty() && peek() != '(' &&
                   priority(peek()) >= priority(*expr))
                output[j++] = pop();
            push(*expr);
        }
        expr++;
    }
    while (!is_empty())
        output[j++] = pop();
    output[j] = '\0';
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <infix-expression>\n", argv[0]);
        return 1;
    }

    char output[MAX_SIZE];
    infix_to_postfix(argv[1], output);
    printf("%s\n", output);

    return 0;
}
