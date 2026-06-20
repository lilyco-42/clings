/*
 * Lesson 35c: 后缀表达式求值 — 参考答案
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 256

int stack[MAX_SIZE];
int top = -1;

void push(int val) { stack[++top] = val; }
int pop(void) { return stack[top--]; }

int eval_postfix(const char *expr)
{
    while (*expr) {
        if (isdigit((unsigned char)*expr)) {
            push(*expr - '0');
        } else {
            int b = pop();
            int a = pop();
            switch (*expr) {
                case '+': push(a + b); break;
                case '-': push(a - b); break;
                case '*': push(a * b); break;
            }
        }
        expr++;
    }
    return pop();
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <postfix-expression>\n", argv[0]);
        return 1;
    }

    printf("%d\n", eval_postfix(argv[1]));

    return 0;
}
