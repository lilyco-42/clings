// 47c_encode.c — 生成 Huffman 编码长度表
//
// 任务: 构建 Huffman 树后，DFS 求出每个字符的编码长度
//       按 ASCII 顺序输出 "c: len"（每行一个字符）
//
// 知识点: 树的 DFS、编码长度 = 节点深度
//
// 预期输出 (args=["aabbbcccc"]):
//   a: 2
//   b: 2
//   c: 1

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
int code_len[MAX_CHARS];  /* 每个字符的编码长度 */

// TODO: 实现 make_leaf, make_internal, find_min（同 47b）

// TODO: 实现 calc_code_lengths 函数
// 功能: DFS 遍历树，叶子节点的 depth 就是编码长度
//   void calc_code_lengths(struct huff_node *root, int depth) {
//       if (!root) return;
//       if (root->ch != -1) { code_len[root->ch] = depth; return; }
//       calc_code_lengths(root->left, depth + 1);
//       calc_code_lengths(root->right, depth + 1);
//   }

// TODO: 实现 free_tree

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    // TODO:
    // 1. 统计频率，构建 Huffman 树（同 47b）
    // 2. 调用 calc_code_lengths(root, 0)
    // 3. 按 ASCII 顺序输出: printf("%c: %d\n", c, code_len[c]);
    // 4. 释放树

    return 0;
}
