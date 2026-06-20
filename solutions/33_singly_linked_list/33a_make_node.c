/*
 * Lesson 33a: 链表节点创建与释放 — 参考答案
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

void delete_node(link p)
{
    free(p->data);
    free(p);
}

int main(void)
{
    char *a = malloc(sizeof(char));
    char *b = malloc(sizeof(char));
    char *c = malloc(sizeof(char));
    *a = 'A';
    *b = 'B';
    *c = 'C';

    link n1 = make_node(a);
    link n2 = make_node(b);
    link n3 = make_node(c);

    printf("%c\n", *(char *)n1->data);
    printf("%c\n", *(char *)n2->data);
    printf("%c\n", *(char *)n3->data);

    delete_node(n1);
    delete_node(n2);
    delete_node(n3);

    return 0;
}
