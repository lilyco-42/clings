/*
 * Lesson 36a: 数组实现队列 — 参考答案
 */
#include <stdio.h>

#define MAX_SIZE 100

int queue[MAX_SIZE];
int front = 0;
int rear = 0;

int is_empty(void) { return front == rear; }
void enqueue(int val) { queue[rear++] = val; }
int dequeue(void) { return queue[front++]; }

int main(void)
{
    enqueue(10);
    enqueue(20);
    enqueue(30);

    while (!is_empty())
        printf("%d\n", dequeue());

    return 0;
}
