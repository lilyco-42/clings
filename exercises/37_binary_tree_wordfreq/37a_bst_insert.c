// 37a_bst_insert.c — BST 插入: 构建二叉搜索树
//
// 任务: 实现 BST 的插入操作，将整数插入到正确位置
//       依次插入 5,3,7,1,4 后中序遍历打印
//
// 知识点: 二叉搜索树的递归插入、左小右大规则
//
// 预期输出:
//   1 3 4 5 7

#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *left;
    struct node *right;
};

// TODO: 实现 make_node 函数
// 功能: 分配新节点，设置 data，左右子树为 NULL
// struct node *make_node(int val) { ... }

// TODO: 实现 insert 函数（递归）
// 功能:
//   - 如果 root 为 NULL，创建新节点返回
//   - 如果 val < root->data，递归插入左子树
//   - 如果 val > root->data，递归插入右子树
//   - 返回 root
// struct node *insert(struct node *root, int val) { ... }

// TODO: 实现 inorder 函数（中序遍历）
// 功能: 递归遍历: 左 → 打印 → 右（元素间用空格分隔，末尾无空格）
//   - 如果 root 为 NULL，返回
//   - 递归 inorder(root->left)
//   - 用全局变量 first 控制空格: 第一个数前不打印空格
//   - printf("%d", root->data)
//   - 递归 inorder(root->right)
// int first = 1;
// void inorder(struct node *root) { ... }

int main(void)
{
    // TODO: 创建空树 root = NULL
    // TODO: 依次 insert 5, 3, 7, 1, 4
    // TODO: 调用 inorder(root) 打印
    // TODO: printf("\n")

    return 0;
}
