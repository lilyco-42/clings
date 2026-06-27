/* 32_singly_list_reverse.c — 单链表反转与快慢指针环检测 【面试高频】
 *
 * 任务: 1. reverse(head)  — 三指针法原地反转单链表，返回新 head
 *       2. has_cycle(head) — Floyd 快慢指针，有环返回 1 无环返回 0
 *       3. 编写 main(): 读一行数字建链表，
 *          若含 "cycle" 则制造环并检测，否则反转并打印
 *
 * 知识点: 三指针反转（prev/curr/next）、Floyd 判圈（slow 1步, fast 2步）
 *
 * 验证（反转 1 2 3 → 3 2 1，有环链表检测）：
 *   stdin: "1 2 3 4 5\n"        → 5 4 3 2 1
 *   stdin: "1\n"                → 1
 *   stdin: "1 2 3\ncycle\n"     → cycle detected
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    int data;
    struct node *next;
};

/* 创建值为 val 的新节点（已实现） */
struct node *make_node(int val) {
    struct node *p = malloc(sizeof(*p));
    p->data = val;
    p->next = NULL;
    return p;
}

/* 尾插建表：返回 head，通过 tail 参数返回尾节点 */
struct node *build_list(char *nums, struct node **tail) {
    struct node *head = NULL;
    *tail = NULL;
    char *tok = strtok(nums, " ");
    while (tok) {
        struct node *p = make_node(atoi(tok));
        if (!head)
            head = *tail = p;
        else {
            (*tail)->next = p;
            *tail = p;
        }
        tok = strtok(NULL, " ");
    }
    return head;
}

/* 打印链表（已实现） */
void list_print(struct node *head) {
    struct node *p = head;
    while (p) {
        printf("%d", p->data);
        p = p->next;
        if (p) printf(" ");
    }
    printf("\n");
}

/* 释放链表（已实现），has_cycle 有环不能直接遍历释放 */
void list_free(struct node *head, int has_cycle) {
    if (has_cycle) return; /* 有环不能直接遍历释放 */
    while (head) {
        struct node *next = head->next;
        free(head);
        head = next;
    }
}

/* 三指针反转：prev（已反转好的）、curr（当前）、next（下一个）
 * 每次把 curr->next 指向 prev，三个指针整体前移 */
struct node *reverse(struct node *head) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 初始化：prev = NULL, curr = head */

    /* while (curr) 循环：
     *   1. 保存下一个节点 next = curr->next
     *   2. 反转：curr->next = prev
     *   3. 前移：prev = curr; curr = next */

    /* 返回 prev（它就是新 head） */
}

/* Floyd 判圈：slow 每次走 1 步，fast 每次走 2 步
 * 如果有环，fast 会追上 slow */
int has_cycle(struct node *head) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 空链表无环 */

    /* slow 和 fast 都从 head 出发 */

    /* while (fast 和 fast->next 都非空) 循环：
     *   slow 走 1 步; fast 走 2 步
     *   如果 slow == fast → 相遇了 → 有环！ */

    /* fast 追到 NULL → 无环 */
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

    /* 在 strtok 破坏字符串前检查 "cycle"! */
    int is_cycle = (strstr(line, "cycle") != NULL);

    struct node *tail;
    struct node *head = build_list(line, &tail);

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 如果有 cycle：
     *   制造环（tail->next = head），调用 has_cycle 检测并打印
     * 否则：
     *   调用 reverse 反转链表，打印结果 */

    list_free(head, is_cycle);
    return 0;
}
