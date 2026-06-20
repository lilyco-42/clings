// 35a_stack_basic.c — 数组实现栈: push/pop/is_empty
//
// 任务: 实现基于数组的栈数据结构
//       push 1, 2, 3 后依次 pop 并打印
//
// 知识点: 栈的 LIFO 特性、数组模拟栈、top 指针
//
// 预期输出:
//   3
//   2
//   1

#include <stdio.h>

#define MAX_SIZE 100

int stack[MAX_SIZE];
int top = -1;

// TODO: 实现 is_empty 函数
// 功能: top == -1 时返回 1，否则返回 0
// int is_empty(void) { ... }

// TODO: 实现 push 函数
// 功能: top++，然后 stack[top] = val
// void push(int val) { ... }

// TODO: 实现 pop 函数
// 功能: 返回 stack[top]，然后 top--
// int pop(void) { ... }

int main(void)
{
    // TODO: push(1); push(2); push(3);
    // TODO: 循环 pop 并打印直到栈空
    //   while (!is_empty())
    //       printf("%d\n", pop());

    return 0;
}
