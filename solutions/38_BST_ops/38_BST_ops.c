/* 38_BST_ops.c — BST操作（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    int data;
    struct node *left, *right;
};
struct node *make_node(int val) {
    struct node *p = malloc(sizeof(*p));
    p->data = val;
    p->left = p->right = NULL;
    return p;
}
void inorder(struct node *root) {
    if (!root) return;
    inorder(root->left);
    printf("%d ", root->data);
    inorder(root->right);
}

struct node *find_min(struct node *root) {
    if (!root) return NULL;
    while (root->left) root = root->left;
    return root;
}
struct node *bst_insert(struct node *root, int val) {
    if (!root) return make_node(val);
    if (val < root->data)
        root->left = bst_insert(root->left, val);
    else if (val > root->data)
        root->right = bst_insert(root->right, val);
    return root;
}
struct node *bst_search(struct node *root, int val) {
    if (!root || root->data == val) return root;
    if (val < root->data) return bst_search(root->left, val);
    return bst_search(root->right, val);
}
struct node *bst_delete(struct node *root, int val) {
    if (!root) return NULL;
    if (val < root->data)
        root->left = bst_delete(root->left, val);
    else if (val > root->data)
        root->right = bst_delete(root->right, val);
    else {
        if (!root->left) {
            struct node *t = root->right;
            free(root);
            return t;
        }
        if (!root->right) {
            struct node *t = root->left;
            free(root);
            return t;
        }
        struct node *min = find_min(root->right);
        root->data = min->data;
        root->right = bst_delete(root->right, min->data);
    }
    return root;
}

int main(void) {
    struct node *root = NULL;
    for (int ln = 0; ln < 2; ln++) {
        char line[256];
        if (!fgets(line, sizeof(line), stdin)) break;
        int i = 0;
        while (line[i] && line[i] != '\n') i++;
        line[i] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "insert") == 0) {
            char *tok;
            while ((tok = strtok(NULL, " "))) root = bst_insert(root, atoi(tok));
            printf("inorder: ");
            inorder(root);
            printf("\n");
        } else if (strcmp(cmd, "search") == 0) {
            int val = atoi(strtok(NULL, " "));
            printf("%s\n", bst_search(root, val) ? "found" : "not found");
        } else if (strcmp(cmd, "delete") == 0) {
            char *tok;
            while ((tok = strtok(NULL, " "))) root = bst_delete(root, atoi(tok));
            printf("inorder: ");
            inorder(root);
            printf("\n");
        }
    }
    return 0;
}
