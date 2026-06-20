/*
 * Lesson 33b: 链表尾插与遍历打印 — 参考答案
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
    link p = head->next;
    while (p) {
        if (p != head->next)
            printf(" -> ");
        pf(p->data);
        p = p->next;
    }
    printf("\n");
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

    list_insert_tail(head, make_node(a));
    list_insert_tail(head, make_node(b));
    list_insert_tail(head, make_node(c));

    list_print(head, print_char);

    return 0;
}
