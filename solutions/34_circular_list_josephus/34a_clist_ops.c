/*
 * Lesson 34a: 循环链表插入与遍历 — 参考答案
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

void clist_print(link cur)
{
    if (cur == NULL) return;
    link p = cur;
    int first = 1;
    do {
        if (!first) printf(" ");
        printf("%d", *(int *)p->data);
        first = 0;
        p = p->next;
    } while (p != cur);
    printf("\n");
}

int main(void)
{
    int n;
    scanf("%d", &n);

    link cur = NULL;
    for (int i = 1; i <= n; i++) {
        link item = make_node(make_int_data(i));
        cur = clist_insert_after(cur, item);
    }

    /* cur 指向最后插入的节点，cur->next 是第一个节点 */
    clist_print(cur->next);

    return 0;
}
