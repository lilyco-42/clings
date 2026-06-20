/*
 * Lesson 37e: BST 删除 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *left;
    struct node *right;
};

struct node *make_node(int val)
{
    struct node *p = malloc(sizeof(struct node));
    p->data = val;
    p->left = NULL;
    p->right = NULL;
    return p;
}

struct node *insert(struct node *root, int val)
{
    if (root == NULL)
        return make_node(val);
    if (val < root->data)
        root->left = insert(root->left, val);
    else if (val > root->data)
        root->right = insert(root->right, val);
    return root;
}

int first = 1;

void inorder(struct node *root)
{
    if (root == NULL)
        return;
    inorder(root->left);
    if (first)
        first = 0;
    else
        printf(" ");
    printf("%d", root->data);
    inorder(root->right);
}

struct node *find_min(struct node *root)
{
    while (root->left != NULL)
        root = root->left;
    return root;
}

struct node *delete_node(struct node *root, int val)
{
    if (root == NULL)
        return NULL;

    if (val < root->data) {
        root->left = delete_node(root->left, val);
    } else if (val > root->data) {
        root->right = delete_node(root->right, val);
    } else {
        /* 找到要删除的节点 */
        if (root->left == NULL) {
            struct node *tmp = root->right;
            free(root);
            return tmp;
        } else if (root->right == NULL) {
            struct node *tmp = root->left;
            free(root);
            return tmp;
        } else {
            /* 有两个子树: 用右子树最小值替换 */
            struct node *successor = find_min(root->right);
            root->data = successor->data;
            root->right = delete_node(root->right, successor->data);
        }
    }
    return root;
}

int main(void)
{
    struct node *root = NULL;
    root = insert(root, 5);
    root = insert(root, 3);
    root = insert(root, 7);
    root = insert(root, 1);
    root = insert(root, 4);

    root = delete_node(root, 3);

    inorder(root);
    printf("\n");

    return 0;
}
