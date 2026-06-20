// 37d_word_freq.c — 单词频率统计: 用 BST 统计单词出现次数
//
// 任务: 从 stdin 读入一行文本，按空格分词
//       用 BST 统计每个单词的出现次数（按字典序输出）
//       格式: "单词:次数"，条目之间用空格分隔
//
// 知识点: 字符串比较 strcmp、BST 存储字符串节点
//
// 示例: 输入 "the cat sat on the mat"
//       输出 "cat:1 mat:1 on:1 sat:1 the:2"
//
// 预期输出（stdin="the cat sat on the mat\n"）:
//   cat:1 mat:1 on:1 sat:1 the:2

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

// TODO: 实现 make_node 函数
// 功能: 分配新节点，strcpy word，count = 1，left = right = NULL
// struct node *make_node(const char *w) { ... }

// TODO: 实现 insert 函数
// 功能:
//   - 如果 root 为 NULL，创建新节点返回
//   - 用 strcmp 比较: < 0 插左, > 0 插右, == 0 则 count++
//   - 返回 root
// struct node *insert(struct node *root, const char *w) { ... }

// TODO: 实现 print_freq 函数
// 功能: 中序遍历打印 "单词:次数"，条目间用空格分隔
// void print_freq(struct node *root, int *first) { ... }

int main(void)
{
    // TODO: 读一行文本 fgets(line, MAX_LINE, stdin)
    // TODO: 用简单循环或 strtok 按空格分词
    // TODO: 每个单词 insert 到 BST
    // TODO: 调用 print_freq 打印
    // TODO: printf("\n")

    return 0;
}
