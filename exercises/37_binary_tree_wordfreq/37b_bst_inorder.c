// 37b_bst_inorder.c — BST 中序遍历: 从字符串构建 BST
//
// 任务: 将字符串 "SEARCHEXAMPLE" 的每个字符插入 BST（忽略重复）
//       中序遍历打印所有唯一字符（按字母序）
//
// 知识点: 字符类型的 BST、中序遍历有序性
//
// 预期输出:
//   A C E H L M P R S X

#include <stdio.h>
#include <stdlib.h>

struct node {
    char ch;
    struct node *left;
    struct node *right;
};

// TODO: 实现 make_node 函数
// 功能: 分配新节点，设置 ch 字段，左右子树为 NULL
// struct node *make_node(char c) { ... }

// TODO: 实现 insert 函数（递归）
// 功能:
//   - 如果 root 为 NULL，创建新节点返回
//   - 如果 c < root->ch，递归插入左子树
//   - 如果 c > root->ch，递归插入右子树
//   - 如果 c == root->ch，忽略（不插入重复字符）
//   - 返回 root
// struct node *insert(struct node *root, char c) { ... }

// TODO: 实现 inorder 函数
// 功能: 中序遍历打印字符，字符之间用空格分隔
//   使用一个 int *first 参数控制第一个字符前不打印空格
// void inorder(struct node *root, int *first) { ... }

int main(void)
{
    // TODO: 定义字符串 str = "SEARCHEXAMPLE"
    // TODO: 将每个字符插入 BST
    // TODO: 中序遍历打印（字符间用空格分隔）
    // TODO: printf("\n")

    return 0;
}
