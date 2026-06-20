// 47b_build_tree.c — 构建 Huffman 树并输出加权路径长度
//
// 任务: 构建 Huffman 树，输出 WPL（加权路径长度 = 总编码位数）
//       从命令行读取字符串 argv[1]
//
// 知识点: Huffman 算法、贪心策略、树结构
//
// 预期输出 (args=["aabbbcccc"]):
//   14

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHARS 256

struct huff_node {
    int freq;
    int ch;          /* 叶子节点的字符，内部节点为 -1 */
    int min_ch;      /* 子树中最小 ASCII 字符，用于确定性排序 */
    struct huff_node *left, *right;
};

struct huff_node *nodes[MAX_CHARS];
int node_count = 0;

// TODO: 实现 make_leaf 函数
// 功能: 创建叶子节点，设置 freq, ch, min_ch, left=right=NULL
// struct huff_node *make_leaf(int ch, int freq) { ... }

// TODO: 实现 make_internal 函数
// 功能: 创建内部节点，合并 left 和 right
//   freq = left->freq + right->freq
//   ch = -1 (非叶子)
//   min_ch = min(left->min_ch, right->min_ch)
// struct huff_node *make_internal(struct huff_node *left, struct huff_node *right) { ... }

// TODO: 实现 find_min 函数
// 功能: 在 nodes[] 中找频率最小的节点索引
//   频率相同时，min_ch 小的优先
//   跳过 NULL 节点
// int find_min(void) { ... }

// TODO: 实现 calc_wpl 函数
// 功能: 递归计算加权路径长度
//   叶子: return freq * depth
//   内部: return calc_wpl(left, depth+1) + calc_wpl(right, depth+1)
// int calc_wpl(struct huff_node *root, int depth) { ... }

// TODO: 实现 free_tree 函数
// void free_tree(struct huff_node *root) { ... }

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    // TODO:
    // 1. 统计字符频率
    // 2. 为每个出现的字符创建叶子节点存入 nodes[]
    // 3. 循环合并: 每次找两个最小节点合并
    //    int m1 = find_min(); nodes[m1] 取出; 置 NULL
    //    int m2 = find_min(); nodes[m2] 取出; 置 NULL
    //    合并后放回 nodes[m1]
    // 4. 计算并打印 WPL
    // 5. 释放树内存

    return 0;
}
