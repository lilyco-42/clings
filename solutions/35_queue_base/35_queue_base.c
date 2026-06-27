/* 35_queue_base.c — 环形队列（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 64

int queue[MAX], front = 0, rear = 0;
int is_empty(void) { return front == rear; }

void enqueue(int val) {
    queue[rear] = val;
    rear = (rear + 1) % MAX;
}
int dequeue(void) {
    int val = queue[front];
    front = (front + 1) % MAX;
    return val;
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    char *tok = strtok(line, " \n");
    while (tok) {
        enqueue(atoi(tok));
        tok = strtok(NULL, " \n");
    }
    int first = 1;
    while (!is_empty()) {
        if (!first) printf(" ");
        printf("%d", dequeue());
        first = 0;
    }
    printf("\n");
    return 0;
}
