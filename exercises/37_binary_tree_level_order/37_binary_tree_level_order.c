/* 37_binary_tree_level_order.c — 二叉树层序遍历（BFS）
 *
 * 任务: 实现 levelorder(root) — 用队列辅助进行层序遍历
 *       复用队列思想（数组 + front/rear），队列中存储 struct node*
 *       按层从上到下、从左到右输出每个节点
 *
 * 知识点: BFS 在树上的应用、队列辅助遍历、泛型元素 struct node*
 *
 * 验证：
 *   stdin: "A B C D E . . . . .\n" → A B C D E
 *   stdin: "A . .\n"               → A
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 256

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

/* 按层序构建二叉树（与 Lesson 36 相同） */
struct node *build_tree(char *tokens) {
    if (!tokens || !*tokens) return NULL;
    char *tok = strtok(tokens, " \n");
    if (!tok || *tok == '.') return NULL;
    struct node *root = make_node(*tok), *queue[MAX];
    int front = 0, rear = 0;
    queue[rear++] = root;
    while (front < rear) {
        struct node *cur = queue[front++];
        tok = strtok(NULL, " \n");
        if (tok && *tok != '.') {
            cur->left = make_node(*tok);
            queue[rear++] = cur->left;
        }
        tok = strtok(NULL, " \n");
        if (tok && *tok != '.') {
            cur->right = make_node(*tok);
            queue[rear++] = cur->right;
        }
    }
    return root;
}

/* 用队列实现层序遍历（BFS）
 * 与 Lesson 35 的队列思想相同，但队列中存的是 struct node* */
void levelorder(struct node *root) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 空树 → 直接返回 */

    /* 创建队列（struct node* 数组 + front/rear 索引） */

    /* 根节点入队 */

    /* while (队列非空) 循环：
     *   出队一个节点 → 打印它
     *   如果它有左孩子 → 左孩子入队
     *   如果它有右孩子 → 右孩子入队 */

    /* 注意输出格式：空格分隔节点 */
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);

    struct node *root = build_tree(line);
    levelorder(root);
    printf("\n");
    return 0;
}
