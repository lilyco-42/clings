/*
 * Lesson 34b: 循环链表删除 — 参考答案
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

link clist_delete(link item)
{
    /* 唯一节点 */
    if (item->next == item) {
        free(item->data);
        free(item);
        return NULL;
    }
    /* 找前驱 */
    link prev = item;
    while (prev->next != item)
        prev = prev->next;
    prev->next = item->next;
    link ret = item->next;
    free(item->data);
    free(item);
    return ret;
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

    /* 找到值为 k 的节点 */
    link head = cur->next;
    link target = head;
    while (*(int *)target->data != k)
        target = target->next;

    /* 删除目标节点 */
    cur = clist_delete(target);

    /* 找到第一个节点（值为 1）来开始打印 */
    link start = cur;
    link p = cur;
    do {
        if (*(int *)p->data == 1) {
            start = p;
            break;
        }
        p = p->next;
    } while (p != cur);

    /* 从值为 1 的节点开始打印 */
    p = start;
    int is_first = 1;
    do {
        if (!is_first) printf(" ");
        printf("%d", *(int *)p->data);
        is_first = 0;
        p = p->next;
    } while (p != start);
    printf("\n");

    return 0;
}
