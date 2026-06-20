/*
 * Lesson 34b: 约瑟夫环问题 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

struct node {
    void *data;
    struct node *next;
};

typedef struct node *link;

link make_node(void *data)
{
    link p = malloc(sizeof(*p));
    p->data = data;
    p->next = NULL;
    return p;
}

int *make_int_data(int val)
{
    int *p = malloc(sizeof(int));
    *p = val;
    return p;
}

link clist_insert_after(link cur, link item)
{
    if (cur == NULL) {
        item->next = item;
        return item;
    }
    item->next = cur->next;
    cur->next = item;
    return item;
}

link clist_delete(link cur, link item)
{
    /* 只剩一个节点 */
    if (item->next == item) {
        free(item->data);
        free(item);
        return NULL;
    }
    /* 找前驱 */
    link p = cur;
    do {
        if (p->next == item) {
            p->next = item->next;
            free(item->data);
            free(item);
            return p;
        }
        p = p->next;
    } while (p != cur);
    return cur;
}

int main(void)
{
    int n, k;
    scanf("%d %d", &n, &k);

    /* 构建循环链表 1..n */
    link cur = NULL;
    for (int i = 1; i <= n; i++) {
        link item = make_node(make_int_data(i));
        cur = clist_insert_after(cur, item);
    }
    /* cur 指向最后一个节点(n)，cur->next 是第一个节点(1) */

    /* 约瑟夫环: 从 cur 开始，每次走 k 步淘汰 */
    int first = 1;
    while (cur != NULL) {
        for (int i = 0; i < k; i++)
            cur = cur->next;
        if (!first) printf(" ");
        printf("%d", *(int *)cur->data);
        first = 0;
        link to_del = cur;
        cur = clist_delete(cur, to_del);
    }
    printf("\n");

    return 0;
}
