/* 36_binary_tree_traversal.c — 二叉树前中后序遍历（递归）
 *
 * 任务: 1. preorder(root)  — 前序: 根→左→右
 *       2. inorder(root)   — 中序: 左→根→右
 *       3. postorder(root) — 后序: 左→右→根
 *       4. 编写 main(): 读一行层序序列（. 表示空），构建二叉树后依次输出三种遍历
 *
 * 知识点: 递归树遍历、DFS 模型、三种遍历的唯一区别是 printf 的位置
 *
 * 验证：
 *   stdin: "A B C . . .\n"  → preorder: A B C
 *                              inorder: B A C
 *                              postorder: B C A
 *   stdin: "A . .\n"        → preorder: A
 *                              inorder: A
 *                              postorder: A
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    char ch;
    struct node *left, *right;
};
struct node *make_node(char ch) {
    struct node *p = malloc(sizeof(*p));
    p->ch = ch;
    p->left = p->right = NULL;
    return p;
}

/* 按层序构建二叉树: token 流，'.' 表示空节点 */
struct node *build_tree(char *tokens) {
    if (!tokens || !*tokens) return NULL;
    char *tok = strtok(tokens, " \n");
    if (!tok || *tok == '.') return NULL;
    struct node *root = make_node(*tok), *queue[256];
    int front = 0, rear = 0;
    queue[rear++] = root;
    while (front < rear) {
        struct node *cur = queue[front++];
        /* 左孩子 */
        tok = strtok(NULL, " \n");
        if (tok && *tok != '.') {
            cur->left = make_node(*tok);
            queue[rear++] = cur->left;
        }
        /* 右孩子 */
        tok = strtok(NULL, " \n");
        if (tok && *tok != '.') {
            cur->right = make_node(*tok);
            queue[rear++] = cur->right;
        }
    }
    return root;
}

/* 前序遍历：根 → 左 → 右 */
void preorder(struct node *root) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 空节点 → return */

    /* 打印当前节点（根） */

    /* 递归左子树 */

    /* 递归右子树 */
}

/* 中序遍历：左 → 根 → 右 */
void inorder(struct node *root) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 空节点 → return */

    /* 递归左子树 */

    /* 打印当前节点（根） */

    /* 递归右子树 */
}

/* 后序遍历：左 → 右 → 根 */
void postorder(struct node *root) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 空节点 → return */

    /* 递归左子树 */

    /* 递归右子树 */

    /* 打印当前节点（根） */
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);

    struct node *root = build_tree(line);

    printf("preorder: ");
    preorder(root);
    printf("\n");

    printf("inorder: ");
    inorder(root);
    printf("\n");

    printf("postorder: ");
    postorder(root);
    printf("\n");

    return 0;
}
