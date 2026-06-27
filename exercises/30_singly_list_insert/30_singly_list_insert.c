/* 30_singly_list_insert.c — 单链表动态插入：头插法与尾插法
 *
 * 任务: 1. 实现 insert_head(head, val) — 头插 O(1), 返回新 head
 *       2. 实现 insert_tail(head, val) — 尾插 O(n), 返回 head
 *       3. 编写 main(): 根据输入模式（head/tail）插入数字并打印链表
 *
 * 知识点: malloc/free、结构体指针、头插/尾插、空链表边界
 *
 * 验证：（头插 1 2 3 → 3 2 1，尾插 1 2 3 → 1 2 3）
 *   stdin: "head 1 2 3\n"  → 3 2 1   （头插逆序）
 *   stdin: "tail 1 2 3\n"  → 1 2 3   （尾插保持原序）
 *   stdin: "head 5\n"      → 5
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

/* 头插法：新节点插入链表头部，成为新的 head
 * 提示：新节点的 next 指向原来的 head */
struct node *insert_head(struct node *head, int val) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 创建值为 val 的新节点 */

    /* 新节点的 next 指向原来链表的第一个节点 */

    /* 返回新节点（它就是新的 head） */
}

/* 尾插法：新节点插入链表尾部
 * 注意空链表特殊情况：head == NULL 时新节点就是 head */
struct node *insert_tail(struct node *head, int val) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 创建值为 val 的新节点 */

    /* 如果链表为空（head == NULL），直接返回新节点 */

    /* 遍历到链表尾部：找到最后一个节点（它的 next == NULL） */

    /* 把尾节点的 next 指向新节点；返回原来的 head（head 没变） */
}

void list_print(struct node *head) {
    struct node *p = head;
    while (p) {
        printf("%d", p->data);
        p = p->next;
        if (p) printf(" ");
    }
    printf("\n");
}

/* 释放链表（已实现） */
void list_free(struct node *head) {
    while (head) {
        struct node *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    /* 解析模式：第一个 token 是 "head" 或 "tail" */
    char *mode = strtok(line, " \n");
    struct node *head = NULL;

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 循环读取后续数字 token（strtok(NULL, " \n")）
     * 每个数字转为 int，根据 mode 决定调用 insert_head 还是 insert_tail
     * 提示：strcmp(mode, "head") == 0 判断模式 */

    list_print(head);
    list_free(head);
    return 0;
}
