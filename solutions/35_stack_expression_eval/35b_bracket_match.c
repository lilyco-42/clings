/*
 * Lesson 35b: 栈实现括号匹配 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 256

char stack[MAX_SIZE];
int top = -1;

void push(char c) { stack[++top] = c; }
char pop(void) { return stack[top--]; }
int is_empty(void) { return top == -1; }

int is_match(char open, char close)
{
    return (open == '(' && close == ')') ||
           (open == '[' && close == ']') ||
           (open == '{' && close == '}');
}

int check_brackets(const char *s)
{
    while (*s) {
        if (*s == '(' || *s == '[' || *s == '{') {
            push(*s);
        } else if (*s == ')' || *s == ']' || *s == '}') {
            if (is_empty()) return 0;
            char open = pop();
            if (!is_match(open, *s)) return 0;
        }
        s++;
    }
    return is_empty();
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage: %s <expression>\n", argv[0]);
        return 1;
    }

    if (check_brackets(argv[1]))
        printf("yes\n");
    else
        printf("no\n");

    return 0;
}
