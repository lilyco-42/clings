// 36a_queue_basic.c — 数组实现队列: enqueue/dequeue/is_empty
//
// 任务: 实现基于数组的队列数据结构
//       enqueue 10, 20, 30 后依次 dequeue 并打印
//
// 知识点: 队列的 FIFO 特性、front/rear 指针
//
// 预期输出:
//   10
//   20
//   30

#include <stdio.h>

#define MAX_SIZE 100

int queue[MAX_SIZE];
int front = 0;
int rear = 0;

// TODO: 实现 is_empty 函数
// 功能: front == rear 时返回 1，否则返回 0
// int is_empty(void) { ... }

// TODO: 实现 enqueue 函数
// 功能: queue[rear] = val; rear++;
// void enqueue(int val) { ... }

// TODO: 实现 dequeue 函数
// 功能: 返回 queue[front]; front++;
// int dequeue(void) { ... }

int main(void)
{
    // TODO: enqueue(10); enqueue(20); enqueue(30);
    // TODO: 循环 dequeue 并打印直到队列空
    //   while (!is_empty())
    //       printf("%d\n", dequeue());

    return 0;
}
