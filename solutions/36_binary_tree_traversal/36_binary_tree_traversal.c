/* 36_binary_tree_traversal.c — 二叉树遍历（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    struct node *root = make_node(*tok), *queue[256];
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

void preorder(struct node *root) {
    if (!root) return;
    printf("%c ", root->ch);
    preorder(root->left);
    preorder(root->right);
}
void inorder(struct node *root) {
    if (!root) return;
    inorder(root->left);
    printf("%c ", root->ch);
    inorder(root->right);
}
void postorder(struct node *root) {
    if (!root) return;
    postorder(root->left);
    postorder(root->right);
    printf("%c ", root->ch);
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    struct node *root = build_tree(line);
    printf("preorder: ");
    preorder(root);
    printf("\n");
    printf("inorder: ");
    inorder(root);
    printf("\n");
    printf("postorder: ");
    postorder(root);
    printf("\n");
    return 0;
}
