/* 37_binary_tree_level_order.c — 层序遍历（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 256

struct node {
    char ch;
    struct node *left, *right;
};
struct node *make_node(char ch) {
    struct node *p = malloc(sizeof(*p));
    p->ch = ch;
    p->left = p->right = NULL;
    return p;
}
struct node *build_tree(char *tokens) {
    if (!tokens || !*tokens) return NULL;
    char *tok = strtok(tokens, " \n");
    if (!tok || *tok == '.') return NULL;
    struct node *root = make_node(*tok), *queue[MAX];
    int front = 0, rear = 0;
    queue[rear++] = root;
    while (front < rear) {
        struct node *cur = queue[front++];
        tok = strtok(NULL, " \n");
        if (tok && *tok != '.') {
            cur->left = make_node(*tok);
            queue[rear++] = cur->left;
        }
        tok = strtok(NULL, " \n");
        if (tok && *tok != '.') {
            cur->right = make_node(*tok);
            queue[rear++] = cur->right;
        }
    }
    return root;
}

void levelorder(struct node *root) {
    if (!root) return;
    struct node *q[MAX];
    int front = 0, rear = 0, first = 1;
    q[rear++] = root;
    while (front < rear) {
        struct node *node = q[front++];
        if (!first) printf(" ");
        printf("%c", node->ch);
        first = 0;
        if (node->left) q[rear++] = node->left;
        if (node->right) q[rear++] = node->right;
    }
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    struct node *root = build_tree(line);
    levelorder(root);
    printf("\n");
    return 0;
}
