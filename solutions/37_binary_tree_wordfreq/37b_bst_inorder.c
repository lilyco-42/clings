/*
 * Lesson 37b: BST 中序遍历 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

struct node {
    char ch;
    struct node *left;
    struct node *right;
};

struct node *make_node(char c)
{
    struct node *p = malloc(sizeof(struct node));
    p->ch = c;
    p->left = NULL;
    p->right = NULL;
    return p;
}

struct node *insert(struct node *root, char c)
{
    if (root == NULL)
        return make_node(c);
    if (c < root->ch)
        root->left = insert(root->left, c);
    else if (c > root->ch)
        root->right = insert(root->right, c);
    /* c == root->ch: 忽略重复 */
    return root;
}

void inorder(struct node *root, int *first)
{
    if (root == NULL)
        return;
    inorder(root->left, first);
    if (*first)
        *first = 0;
    else
        printf(" ");
    printf("%c", root->ch);
    inorder(root->right, first);
}

int main(void)
{
    const char *str = "SEARCHEXAMPLE";
    struct node *root = NULL;

    for (int i = 0; str[i] != '\0'; i++)
        root = insert(root, str[i]);

    int first = 1;
    inorder(root, &first);
    printf("\n");

    return 0;
}
