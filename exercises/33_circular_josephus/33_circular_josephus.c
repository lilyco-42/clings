/* 33_circular_josephus.c — 循环链表与约瑟夫环问题
 *
 * 任务: 1. clist_insert(cur, val) — 循环链表插入
 *          首节点自环（item->next = item），后续节点插在 cur 之后
 *       2. clist_delete(item)      — 删除 item，返回后继
 *          唯一节点返回 NULL
 *       3. 编写 main(): 读 N K，构建 1..N 循环链表，模拟约瑟夫环淘汰
 *
 * 约瑟夫环规则: N 个人围成圈，从 1 开始数，每数到 K 的人出局，直到所有人出局
 *
 * 知识点: 循环链表（自环初始化）、do-while 遍历、cursor 游标、动态删除
 *
 * 验证：
 *   stdin: "10 3\n"  → 3 6 9 2 7 1 8 5 10 4
 *   stdin: "5 2\n"   → 2 4 1 5 3
 */
#include <stdio.h>
#include <stdlib.h>

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

/* 循环链表插入：首节点自环，后续节点插在 cur 之后，返回新节点（作为新 cursor） */
struct node *clist_insert(struct node *cur, int val) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 创建新节点 p */

    /* 如果是第一个节点（cur == NULL）：p 指向自己（自环） */

    /* 否则：插在 cur 之后（p->next = cur->next; cur->next = p） */

    /* 返回 p（新节点成为新的 cursor） */
}

/* 循环链表删除：删除 item，返回后继节点（淘汰后下一轮从这里开始） */
struct node *clist_delete(struct node *item) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 空节点返回 NULL */

    /* 唯一节点（item->next == item）：free(item) 后返回 NULL */

    /* 找到 item 的前驱节点：从 item 出发，一直找直到 next == item */

    /* 前驱跳过 item：prev->next = item->next; free(item)

     * 返回 item->next（被删节点的后继，作为下一轮的起点） */
}

int main(void) {
    int N, K;
    char line[64];
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d %d", &N, &K);

    /* 构建循环链表 1 → 2 → ... → N → 回到 1 */
    struct node *cursor = NULL;
    for (int i = 1; i <= N; i++) cursor = clist_insert(cursor, i);
    /* 移到第一个节点作为起点 */
    cursor = cursor->next;

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 约瑟夫环淘汰过程：
     *   while (cursor != NULL) {
     *       走 K-1 步（因为当前站的那个人已经算 1 了）
     *       打印被删除节点的 data
     *       删除节点，cursor 指向后继
     *       适当控制空格分隔
     *   } */

    printf("\n");
    return 0;
}
