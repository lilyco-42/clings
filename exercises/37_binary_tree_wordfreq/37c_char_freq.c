// 37c_char_freq.c — 字符频率统计: 用 BST 统计字符出现次数
//
// 任务: 从 stdin 读入一行文本，统计每个字母的出现次数
//       用 BST 存储（按字母序输出）
//       格式: "字符:次数"，条目之间用空格分隔
//       只统计小写字母 a-z（忽略换行、空格等）
//
// 知识点: BST 节点增加 count 字段、插入时更新计数
//
// 示例: 输入 "hello" → 输出 "e:1 h:1 l:2 o:1"
//
// 预期输出（stdin="hello\n"）:
//   e:1 h:1 l:2 o:1

#include <stdio.h>
#include <stdlib.h>

struct node {
    char ch;
    int count;
    struct node *left;
    struct node *right;
};

// TODO: 实现 make_node 函数
// 功能: 分配新节点，ch = c, count = 1, left = right = NULL
// struct node *make_node(char c) { ... }

// TODO: 实现 insert 函数
// 功能:
//   - 如果 root 为 NULL，创建新节点返回
//   - 如果 c < root->ch，递归插入左子树
//   - 如果 c > root->ch，递归插入右子树
//   - 如果 c == root->ch，root->count++（计数加一）
//   - 返回 root
// struct node *insert(struct node *root, char c) { ... }

// TODO: 实现 print_freq 函数
// 功能: 中序遍历打印 "字符:次数"，条目间用空格分隔
// void print_freq(struct node *root, int *first) { ... }

int main(void)
{
    // TODO: 从 stdin 逐字符读取（用 getchar()）
    // TODO: 只处理 'a' <= c <= 'z' 的字符
    // TODO: 每个字符 insert 到 BST
    // TODO: 调用 print_freq 打印结果
    // TODO: printf("\n")

    return 0;
}
