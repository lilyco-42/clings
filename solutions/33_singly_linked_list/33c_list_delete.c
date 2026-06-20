/*
 * Lesson 33c: 链表节点删除 — 参考答案
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

link list_delete(link head, link item)
{
    link p = head;
    while (p && p->next != item)
        p = p->next;
    if (p) {
        p->next = item->next;
        free(item->data);
        free(item);
    }
    return head;
}

void print_char(void *data)
{
    if (data)
        printf("%c", *(char *)data);
}

int main(void)
{
    link head = make_node(NULL);

    char *a = malloc(1); *a = 'a';
    char *b = malloc(1); *b = 'b';
    char *c = malloc(1); *c = 'c';

    link na = make_node(a);
    link nb = make_node(b);
    link nc = make_node(c);

    list_insert_tail(head, na);
    list_insert_tail(head, nb);
    list_insert_tail(head, nc);

    /* 删除中间节点 nb */
    list_delete(head, nb);

    /* 打印链表 */
    link p = head->next;
    while (p) {
        if (p != head->next) printf(" -> ");
        print_char(p->data);
        p = p->next;
    }
    printf("\n");

    return 0;
}
