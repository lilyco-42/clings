// 37e_bst_delete.c — BST 删除: 从二叉搜索树中删除节点
//
// 任务: 实现 BST 的删除操作
//       插入 5,3,7,1,4 后删除 3，中序遍历打印
//
// 知识点: BST 删除的三种情况:
//   1. 叶子节点 → 直接删除
//   2. 只有一个子树 → 用子树替换
//   3. 有两个子树 → 用中序后继（右子树最小值）替换
//
// 预期输出:
//   1 4 5 7

#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node *left;
    struct node *right;
};

struct node *make_node(int val)
{
    struct node *p = malloc(sizeof(struct node));
    p->data = val;
    p->left = NULL;
    p->right = NULL;
    return p;
}

struct node *insert(struct node *root, int val)
{
    if (root == NULL)
        return make_node(val);
    if (val < root->data)
        root->left = insert(root->left, val);
    else if (val > root->data)
        root->right = insert(root->right, val);
    return root;
}

int first = 1;

void inorder(struct node *root)
{
    if (root == NULL)
        return;
    inorder(root->left);
    if (first)
        first = 0;
    else
        printf(" ");
    printf("%d", root->data);
    inorder(root->right);
}

// TODO: 实现 find_min 函数
// 功能: 找到以 root 为根的子树中的最小节点（一直往左走）
// struct node *find_min(struct node *root) { ... }

// TODO: 实现 delete_node 函数（递归）
// 功能:
//   - 如果 root 为 NULL，返回 NULL
//   - 如果 val < root->data，递归删除左子树
//   - 如果 val > root->data，递归删除右子树
//   - 如果 val == root->data:
//     a. 无左子树 → 返回右子树（free 当前节点）
//     b. 无右子树 → 返回左子树（free 当前节点）
//     c. 都有 → 找右子树最小值，复制到当前节点，递归删除右子树中的那个值
//   - 返回 root
// struct node *delete_node(struct node *root, int val) { ... }

int main(void)
{
    struct node *root = NULL;
    root = insert(root, 5);
    root = insert(root, 3);
    root = insert(root, 7);
    root = insert(root, 1);
    root = insert(root, 4);

    // TODO: root = delete_node(root, 3);
    // TODO: inorder(root);
    // TODO: printf("\n");

    return 0;
}
