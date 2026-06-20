/*
 * Lesson 33d: 函数指针遍历打印链表 — 参考答案
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

link list_insert_tail(link head, link item)
{
    link p = head;
    while (p->next != NULL)
        p = p->next;
    p->next = item;
    item->next = NULL;
    return head;
}

void list_print(link head, void (*pf)(void *))
{
    link p = head->next;  /* 跳过哨兵 */
    while (p) {
        pf(p->data);
        p = p->next;
    }
    printf("\n");
}

void print_int(void *data)
{
    printf("%d ", *(int *)data);
}

int main(void)
{
    link head = make_node(NULL);

    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;

    list_insert_tail(head, make_node(a));
    list_insert_tail(head, make_node(b));
    list_insert_tail(head, make_node(c));

    list_print(head, print_int);

    return 0;
}
