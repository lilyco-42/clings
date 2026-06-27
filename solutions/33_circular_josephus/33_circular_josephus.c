/* 33_circular_josephus.c — 循环链表与约瑟夫环问题（参考解答） */
#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *next;
};
struct node *make_node(int val) {
    struct node *p = malloc(sizeof(*p));
    p->data = val;
    p->next = NULL;
    return p;
}

struct node *clist_insert(struct node *cur, int val) {
    struct node *p = make_node(val);
    if (!cur) {
        p->next = p;
        return p;
    }
    p->next = cur->next;
    cur->next = p;
    return p;
}

struct node *clist_delete(struct node *item) {
    if (!item) return NULL;
    if (item->next == item) {
        free(item);
        return NULL;
    }
    struct node *prev = item;
    while (prev->next != item) prev = prev->next;
    prev->next = item->next;
    free(item);
    return prev->next;
}

int main(void) {
    int N, K;
    char line[64];
    fgets(line, sizeof(line), stdin);
    sscanf(line, "%d %d", &N, &K);
    struct node *cursor = NULL;
    for (int i = 1; i <= N; i++) cursor = clist_insert(cursor, i);
    cursor = cursor->next;
    int first = 1;
    while (cursor) {
        for (int j = 1; j < K; j++) cursor = cursor->next;
        if (!first) printf(" ");
        printf("%d", cursor->data);
        first = 0;
        cursor = clist_delete(cursor);
    }
    printf("\n");
    return 0;
}
