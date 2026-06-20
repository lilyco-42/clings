/*
 * Lesson 37c: 字符频率统计 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

struct node {
    char ch;
    int count;
    struct node *left;
    struct node *right;
};

struct node *make_node(char c)
{
    struct node *p = malloc(sizeof(struct node));
    p->ch = c;
    p->count = 1;
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
    else
        root->count++;
    return root;
}

void print_freq(struct node *root, int *first)
{
    if (root == NULL)
        return;
    print_freq(root->left, first);
    if (*first)
        *first = 0;
    else
        printf(" ");
    printf("%c:%d", root->ch, root->count);
    print_freq(root->right, first);
}

int main(void)
{
    struct node *root = NULL;
    int c;

    while ((c = getchar()) != EOF) {
        if (c >= 'a' && c <= 'z')
            root = insert(root, (char)c);
    }

    int first = 1;
    print_freq(root, &first);
    printf("\n");

    return 0;
}
