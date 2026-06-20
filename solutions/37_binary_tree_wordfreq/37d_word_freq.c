/*
 * Lesson 37d: 单词频率统计 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WORD 64
#define MAX_LINE 1024

struct node {
    char word[MAX_WORD];
    int count;
    struct node *left;
    struct node *right;
};

struct node *make_node(const char *w)
{
    struct node *p = malloc(sizeof(struct node));
    strcpy(p->word, w);
    p->count = 1;
    p->left = NULL;
    p->right = NULL;
    return p;
}

struct node *insert(struct node *root, const char *w)
{
    if (root == NULL)
        return make_node(w);
    int cmp = strcmp(w, root->word);
    if (cmp < 0)
        root->left = insert(root->left, w);
    else if (cmp > 0)
        root->right = insert(root->right, w);
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
    printf("%s:%d", root->word, root->count);
    print_freq(root->right, first);
}

int main(void)
{
    char line[MAX_LINE];
    struct node *root = NULL;

    if (fgets(line, MAX_LINE, stdin) == NULL)
        return 0;

    /* 去掉末尾换行 */
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
        line[len - 1] = '\0';

    /* 按空格分词 */
    char *token = strtok(line, " ");
    while (token != NULL) {
        root = insert(root, token);
        token = strtok(NULL, " ");
    }

    int first = 1;
    print_freq(root, &first);
    printf("\n");

    return 0;
}
