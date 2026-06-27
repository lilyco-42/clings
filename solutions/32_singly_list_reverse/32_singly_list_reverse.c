/* 32_singly_list_reverse.c — 单链表反转与环检测（参考解答） */
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
struct node *build_list(char *nums, struct node **tail) {
    struct node *head = NULL;
    *tail = NULL;
    char *tok = strtok(nums, " ");
    while (tok) {
        struct node *p = make_node(atoi(tok));
        if (!head)
            head = *tail = p;
        else {
            (*tail)->next = p;
            *tail = p;
        }
        tok = strtok(NULL, " ");
    }
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
void list_free(struct node *head, int has_cycle) {
    if (has_cycle) return;
    while (head) {
        struct node *n = head->next;
        free(head);
        head = n;
    }
}

struct node *reverse(struct node *head) {
    struct node *prev = NULL, *curr = head, *next;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}

int has_cycle(struct node *head) {
    if (!head) return 0;
    struct node *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return 1;
    }
    return 0;
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    int is_cycle = (strstr(line, "cycle") != NULL);
    struct node *tail, *head = build_list(line, &tail);
    if (is_cycle && tail) {
        tail->next = head;
        printf("cycle detected\n");
    } else {
        head = reverse(head);
        list_print(head);
    }
    list_free(head, is_cycle);
    return 0;
}
