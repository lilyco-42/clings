/*
 * Lesson 34a: 循环链表插入 — 参考答案
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

int clist_length(link cur)
{
    if (cur == NULL) return 0;
    int count = 0;
    link p = cur;
    do {
        count++;
        p = p->next;
    } while (p != cur);
    return count;
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

    printf("%d\n", clist_length(cur));

    return 0;
}
