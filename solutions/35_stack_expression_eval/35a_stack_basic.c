/*
 * Lesson 35a: 数组实现栈 — 参考答案
 */
#include <stdio.h>

#define MAX_SIZE 100

int stack[MAX_SIZE];
int top = -1;

int is_empty(void) { return top == -1; }
void push(int val) { stack[++top] = val; }
int pop(void) { return stack[top--]; }

int main(void)
{
    push(1);
    push(2);
    push(3);

    while (!is_empty())
        printf("%d\n", pop());

    return 0;
}
