/* 30_singly_list_insert.c — 单链表动态插入（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

struct node *insert_head(struct node *head, int val) {
    struct node *p = make_node(val);
    p->next = head;
    return p;
}

struct node *insert_tail(struct node *head, int val) {
    struct node *p = make_node(val);
    if (!head) return p;
    struct node *cur = head;
    while (cur->next) cur = cur->next;
    cur->next = p;
    return head;
}

void list_print(struct node *head) {
    struct node *p = head;
    while (p) {
        printf("%d", p->data);
        p = p->next;
        if (p) printf(" ");
    }
    printf("\n");
}

void list_free(struct node *head) {
    while (head) {
        struct node *next = head->next;
        free(head);
        head = next;
    }
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    char *mode = strtok(line, " \n");
    struct node *head = NULL;
    char *tok;
    while ((tok = strtok(NULL, " \n"))) {
        int val = atoi(tok);
        if (strcmp(mode, "head") == 0)
            head = insert_head(head, val);
        else
            head = insert_tail(head, val);
    }
    list_print(head);
    list_free(head);
    return 0;
}
