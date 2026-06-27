/* 34_bracket_match.c — 栈实现括号匹配（参考解答） */
#include <stdio.h>
#define MAX 1024

int is_empty(int top) { return top == -1; }
void push(char *stack, int *top, char c) { stack[++(*top)] = c; }
char pop(char *stack, int *top) { return stack[(*top)--]; }

int is_match(char open, char close) {
    return (open == '(' && close == ')') || (open == '[' && close == ']') || (open == '{' && close == '}');
}

int check_brackets(const char *s) {
    char stack[MAX];
    int top = -1;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '(' || s[i] == '[' || s[i] == '{')
            push(stack, &top, s[i]);
        else if (s[i] == ')' || s[i] == ']' || s[i] == '}') {
            if (is_empty(top) || !is_match(pop(stack, &top), s[i])) return 0;
        }
    }
    return is_empty(top);
}

int main(void) {
    char s[MAX];
    fgets(s, sizeof(s), stdin);
    int i = 0;
    while (s[i] && s[i] != '\n') i++;
    s[i] = '\0';
    printf("%s\n", check_brackets(s) ? "yes" : "no");
    return 0;
}
