/*
 * Lesson 47b: 构建 Huffman 树并计算加权路径长度 — 参考答案
 *
 * 约定: 合并时频率小的为左子树，频率大的为右子树
 *       频率相同时，字符 ASCII 值小的优先（或包含更小字符的子树优先）
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHARS 256

struct huff_node {
    int freq;
    int ch;          /* 叶子节点的字符，内部节点为 -1 */
    int min_ch;      /* 子树中最小字符，用于稳定排序 */
    struct huff_node *left, *right;
};

struct huff_node *nodes[MAX_CHARS];
int node_count = 0;

struct huff_node *make_leaf(int ch, int freq)
{
    struct huff_node *n = malloc(sizeof(*n));
    n->freq = freq;
    n->ch = ch;
    n->min_ch = ch;
    n->left = n->right = NULL;
    return n;
}

struct huff_node *make_internal(struct huff_node *left, struct huff_node *right)
{
    struct huff_node *n = malloc(sizeof(*n));
    n->freq = left->freq + right->freq;
    n->ch = -1;
    n->min_ch = left->min_ch < right->min_ch ? left->min_ch : right->min_ch;
    n->left = left;
    n->right = right;
    return n;
}

/* 找到频率最小的节点（频率相同则 min_ch 小的优先） */
int find_min(void)
{
    int min_idx = -1;
    for (int i = 0; i < node_count; i++) {
        if (nodes[i] == NULL) continue;
        if (min_idx == -1 ||
            nodes[i]->freq < nodes[min_idx]->freq ||
            (nodes[i]->freq == nodes[min_idx]->freq &&
             nodes[i]->min_ch < nodes[min_idx]->min_ch)) {
            min_idx = i;
        }
    }
    return min_idx;
}

/* 计算加权路径长度 (WPL) = 所有叶子的 freq * depth 之和 */
int calc_wpl(struct huff_node *root, int depth)
{
    if (!root) return 0;
    if (root->ch != -1)  /* 叶子 */
        return root->freq * depth;
    return calc_wpl(root->left, depth + 1) + calc_wpl(root->right, depth + 1);
}

void free_tree(struct huff_node *root)
{
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    const char *s = argv[1];
    int freq[MAX_CHARS] = {0};

    for (int i = 0; s[i]; i++)
        freq[(unsigned char)s[i]]++;

    /* 创建叶子节点 */
    node_count = 0;
    for (int c = 0; c < MAX_CHARS; c++) {
        if (freq[c] > 0)
            nodes[node_count++] = make_leaf(c, freq[c]);
    }

    /* 特殊情况: 只有一种字符 */
    if (node_count == 1) {
        printf("%d\n", nodes[0]->freq);
        free(nodes[0]);
        return 0;
    }

    /* 构建 Huffman 树 */
    while (1) {
        int m1 = find_min();
        struct huff_node *n1 = nodes[m1];
        nodes[m1] = NULL;

        int m2 = find_min();
        if (m2 == -1) {
            /* 只剩一个节点，即根 */
            nodes[m1] = n1;
            break;
        }
        struct huff_node *n2 = nodes[m2];
        nodes[m2] = NULL;

        /* 小频率/小字符为左子树 */
        struct huff_node *merged = make_internal(n1, n2);
        nodes[m1] = merged;
    }

    /* 找到根节点 */
    struct huff_node *root = NULL;
    for (int i = 0; i < node_count; i++) {
        if (nodes[i]) { root = nodes[i]; break; }
    }

    int wpl = calc_wpl(root, 0);
    printf("%d\n", wpl);

    free_tree(root);
    return 0;
}
