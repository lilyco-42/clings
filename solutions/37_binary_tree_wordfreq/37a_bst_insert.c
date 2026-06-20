/*
 * Lesson 37a: BST 插入 — 参考答案
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

int main(void)
{
    struct node *root = NULL;
    root = insert(root, 5);
    root = insert(root, 3);
    root = insert(root, 7);
    root = insert(root, 1);
    root = insert(root, 4);

    inorder(root);
    printf("\n");

    return 0;
}
