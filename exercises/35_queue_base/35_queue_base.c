/* 35_queue_base.c — 环形队列基础
 *
 * 任务: 1. enqueue(val) — 入队：在 rear 放值，rear 取模后移
 *       2. dequeue()    — 出队：从 front 取值，front 取模后移
 *       3. 编写 main(): 读一行数字入队，再全部出队打印
 *
 * 环形队列: 用数组 + front/rear 双指针 + 取模实现环绕
 *           广泛用于内核环形缓冲区、音频/网络环形 buffer
 *
 * 知识点: 取模环绕 (ptr+1)%MAX、队空判断 (front==rear)、FIFO 语义
 *
 * 验证：
 *   stdin: "10 20 30\n" → 10 20 30
 *   stdin: "1 2\n"      → 1 2
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 64

int queue[MAX];
int front = 0, rear = 0; /* front == rear 表示队空 */

int is_empty(void) { return front == rear; }

/* 入队：把 val 存到 rear 位置，然后 rear 取模后移 */
void enqueue(int val) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 把 val 放进 queue[rear] */

    /* rear = (rear + 1) % MAX   — 取模实现环绕 */
}

/* 出队：从 front 位置取出值，然后 front 取模后移 */
int dequeue(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 从 queue[front] 取出值保存 */

    /* front = (front + 1) % MAX   — 取模实现环绕 */

    /* 返回保存的值 */
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);

    /* 解析数字并入队 */
    char *tok = strtok(line, " \n");
    while (tok) {
        enqueue(atoi(tok));
        tok = strtok(NULL, " \n");
    }

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 全部出队并打印，空格分隔，最后换行 */

    return 0;
}
