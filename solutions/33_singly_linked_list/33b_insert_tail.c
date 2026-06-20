/*
 * Lesson 33b: 链表尾插法 — 参考答案
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

void print_char(void *data)
{
    if (data)
        printf("%c", *(char *)data);
}

int main(void)
{
    link head = make_node(NULL);

    char *x = malloc(1); *x = 'x';
    char *y = malloc(1); *y = 'y';
    char *z = malloc(1); *z = 'z';

    list_insert_tail(head, make_node(x));
    list_insert_tail(head, make_node(y));
    list_insert_tail(head, make_node(z));

    link p = head->next;
    while (p) {
        if (p != head->next) printf(" -> ");
        print_char(p->data);
        p = p->next;
    }
    printf("\n");

    return 0;
}
