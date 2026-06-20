// 47d_compress.c — 计算 Huffman 压缩率
//
// 任务: 构建 Huffman 树，计算原始大小和编码大小，输出压缩率
//
// 知识点: 信息论基础、压缩效率评估
//
// 预期输出 (args=["aabbbcccc"]):
//   original: 72 bits
//   encoded: 14 bits
//   ratio: 19.4%

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

// TODO: 实现 Huffman 树的所有函数（同 47b/47c）
// make_leaf, make_internal, find_min, calc_code_lengths, free_tree

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    // TODO:
    // 1. 统计频率，构建 Huffman 树
    // 2. 计算编码长度
    // 3. 计算:
    //    int original = strlen(s) * 8;  // 每字符 8 位
    //    int encoded = Σ(freq[c] * code_len[c]);
    //    double ratio = (double)encoded / original * 100.0;
    // 4. 输出:
    //    printf("original: %d bits\n", original);
    //    printf("encoded: %d bits\n", encoded);
    //    printf("ratio: %.1f%%\n", ratio);

    return 0;
}
