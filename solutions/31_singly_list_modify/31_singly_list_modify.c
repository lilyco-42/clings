/* 31_singly_list_modify.c — 单链表查找与删除（参考解答） */
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
struct node *build_list(char *nums) {
    struct node *head = NULL, *tail = NULL;
    char *tok = strtok(nums, " ");
    while (tok) {
        struct node *p = make_node(atoi(tok));
        if (!head)
            head = tail = p;
        else {
            tail->next = p;
            tail = p;
        }
        tok = strtok(NULL, " ");
    }
    return head;
}
void list_print(struct node *head) {
    if (!head) {
        printf("(empty)\n");
        return;
    }
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

struct node *find_by_value(struct node *head, int val) {
    struct node *p = head;
    while (p) {
        if (p->data == val) return p;
        p = p->next;
    }
    return NULL;
}

struct node *find_by_index(struct node *head, int idx) {
    struct node *p = head;
    int i = 0;
    while (p && i < idx) {
        p = p->next;
        i++;
    }
    return p;
}

struct node *delete_node(struct node *head, int val) {
    if (!head) return NULL;
    if (head->data == val) {
        struct node *n = head->next;
        free(head);
        return n;
    }
    struct node *prev = head, *cur = head->next;
    while (cur) {
        if (cur->data == val) {
            prev->next = cur->next;
            free(cur);
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    return head;
}

int main(void) {
    char nums[256], cmd[256];
    fgets(nums, sizeof(nums), stdin);
    fgets(cmd, sizeof(cmd), stdin);
    for (int i = 0; nums[i]; i++)
        if (nums[i] == '\n') nums[i] = '\0';
    for (int i = 0; cmd[i]; i++)
        if (cmd[i] == '\n') cmd[i] = '\0';

    char nums_copy[256];
    strcpy(nums_copy, nums);
    struct node *head = build_list(nums_copy);

    char op[16];
    int val;
    sscanf(cmd, "%s %d", op, &val);

    if (strcmp(op, "find") == 0) {
        struct node *p = find_by_value(head, val);
        if (p)
            printf("found: %d\n", p->data);
        else
            printf("not found\n");
    } else if (strcmp(op, "index") == 0) {
        struct node *p = find_by_index(head, val);
        if (p)
            printf("at %d: %d\n", val, p->data);
        else
            printf("out of range\n");
    } else if (strcmp(op, "delete") == 0) {
        head = delete_node(head, val);
        list_print(head);
    }
    list_free(head);
    return 0;
}
