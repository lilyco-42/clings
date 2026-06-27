/* 31_singly_list_modify.c — 单链表查找与删除
 *
 * 任务: 实现三个函数，操作单链表
 *       1. find_by_value(head, val) — 按值查找，返回第一个匹配节点，未找到返回 NULL
 *       2. find_by_index(head, idx) — 按索引查找（0-based），越界返回 NULL
 *       3. delete_node(head, val)   — 按值删除第一个匹配节点，返回（可能更新的）head
 *          正确处理删头节点、删尾节点、删唯一节点等边界
 *
 * 知识点: 链表遍历、前驱指针 prev、free 释放、空/单节点边界
 *
 * 验证：
 *   stdin: "1 2 3 4\nfind 3\n"     → found: 3
 *   stdin: "1 2 3 4\nfind 5\n"     → not found
 *   stdin: "1 2 3 4\nindex 2\n"    → at 2: 3
 *   stdin: "1 2 3 4\nindex 5\n"    → out of range
 *   stdin: "1 2 3 4\ndelete 2\n"   → 1 3 4
 *   stdin: "1\ndelete 1\n"         → (empty)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    int data;
    struct node *next;
};
struct node *make_node(int val) {
    struct node *p = malloc(sizeof(*p));
    p->data = val;
    p->next = NULL;
    return p;
}

/* 尾插建表（已实现） */
struct node *build_list(char *nums) {
    struct node *head = NULL, *tail = NULL;
    char *tok = strtok(nums, " ");
    while (tok) {
        struct node *p = make_node(atoi(tok));
        if (!head)
            head = tail = p;
        else {
            tail->next = p;
            tail = p;
        }
        tok = strtok(NULL, " ");
    }
    return head;
}

/* 打印链表（已实现）  */
void list_print(struct node *head) {
    if (!head) {
        printf("(empty)\n");
        return;
    }
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

/* 按值查找：遍历链表，找到第一个 data == val 的节点 */
struct node *find_by_value(struct node *head, int val) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 从 head 开始，逐个检查节点的 data 是否等于 val */

    /* 找到就返回该节点；遍历完还没找到返回 NULL */
}

/* 按索引查找（0-based）：返回第 idx 个节点（首节点 idx=0） */
struct node *find_by_index(struct node *head, int idx) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 从头开始，每次移动指针同时计数 */

    /* 索引越界（idx 超出链表长度）时返回 NULL */
}

/* 按值删除：删除第一个匹配节点
 * 关键技巧：维护前驱指针 prev，这样才能从 prev->next 跳过被删节点
 * 特别注意：如果被删的是头节点，需要返回新的 head */
struct node *delete_node(struct node *head, int val) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 空链表：返回 NULL */

    /* 头节点就是目标：保存 head->next，free(head)，返回新 head */

    /* 维护两个指针：prev（前驱）、cur（当前），从 prev=head, cur=head->next 开始 */

    /* 遍历到 cur == NULL 或找到目标：

     *   如果找到：prev->next = cur->next; free(cur); 跳出

     *   否则：prev = cur; cur = cur->next */

    /* 返回 head */
}

int main(void) {
    char nums[256], cmd[256];
    fgets(nums, sizeof(nums), stdin);
    fgets(cmd, sizeof(cmd), stdin);
    /* 去掉换行符 */
    for (int i = 0; nums[i]; i++)
        if (nums[i] == '\n') nums[i] = '\0';
    for (int i = 0; cmd[i]; i++)
        if (cmd[i] == '\n') cmd[i] = '\0';

    /* build_list 会修改原字符串，需要副本 */
    char nums_copy[256];
    strcpy(nums_copy, nums);
    struct node *head = build_list(nums_copy);

    char op[16];
    int val;
    sscanf(cmd, "%s %d", op, &val);

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 根据 op 分发：
     *   "find"   → find_by_value → 打印 found: N 或 not found
     *   "index"  → find_by_index → 打印 at idx: N 或 out of range
     *   "delete" → delete_node    → list_print(head) */

    list_free(head);
    return 0;
}
