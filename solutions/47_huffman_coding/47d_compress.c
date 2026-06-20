/*
 * Lesson 47d: 计算 Huffman 压缩率 — 参考答案
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHARS 256

struct huff_node {
    int freq;
    int ch;
    int min_ch;
    struct huff_node *left, *right;
};

struct huff_node *nodes[MAX_CHARS];
int node_count = 0;
int code_len[MAX_CHARS];

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

void calc_code_lengths(struct huff_node *root, int depth)
{
    if (!root) return;
    if (root->ch != -1) {
        code_len[root->ch] = depth;
        return;
    }
    calc_code_lengths(root->left, depth + 1);
    calc_code_lengths(root->right, depth + 1);
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
    int len = (int)strlen(s);
    int freq[MAX_CHARS] = {0};
    memset(code_len, 0, sizeof(code_len));

    for (int i = 0; s[i]; i++)
        freq[(unsigned char)s[i]]++;

    node_count = 0;
    for (int c = 0; c < MAX_CHARS; c++) {
        if (freq[c] > 0)
            nodes[node_count++] = make_leaf(c, freq[c]);
    }

    if (node_count == 1) {
        /* 只有一种字符，每个字符用 1 bit */
        int original = len * 8;
        int encoded = len;
        printf("original: %d bits\n", original);
        printf("encoded: %d bits\n", encoded);
        printf("ratio: %.1f%%\n", (double)encoded / original * 100.0);
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
            nodes[m1] = n1;
            break;
        }
        struct huff_node *n2 = nodes[m2];
        nodes[m2] = NULL;

        struct huff_node *merged = make_internal(n1, n2);
        nodes[m1] = merged;
    }

    struct huff_node *root = NULL;
    for (int i = 0; i < node_count; i++) {
        if (nodes[i]) { root = nodes[i]; break; }
    }

    calc_code_lengths(root, 0);

    /* 计算总编码位数 */
    int encoded = 0;
    for (int c = 0; c < MAX_CHARS; c++) {
        if (freq[c] > 0)
            encoded += freq[c] * code_len[c];
    }

    int original = len * 8;
    printf("original: %d bits\n", original);
    printf("encoded: %d bits\n", encoded);
    printf("ratio: %.1f%%\n", (double)encoded / original * 100.0);

    free_tree(root);
    return 0;
}
