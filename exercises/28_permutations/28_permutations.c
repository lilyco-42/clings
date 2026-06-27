/* 28_permutations.c — 全排列问题：回溯算法（DFS）入门
 *
 * 任务: 实现 permute(str, l, r) — 输出字符串全排列，每行一个
 *       这是经典的回溯算法（Backtracking / DFS）入门题
 *       为后续八皇后问题（Lesson 29）打底
 *
 * 算法: swap + 递归 + 回溯（回溯三步曲）
 *       1. 固定位置 l，枚举候选字符 i ∈ [l, r]
 *       2. swap(str[l], str[i]) — 把候选换到固定位置
 *       3. 递归处理子问题 permute(str, l+1, r)
 *       4. swap(str[l], str[i]) — 换回来！（回溯）
 *       5. 当 l == r 时到达叶子节点，打印当前排列
 *
 * 知识点: 递归、回溯（swap-递归-swap模式）、DFS 排列树
 *
 * 验证：
 *   stdin: "ABC\n"  → ABC ACB BAC BCA CBA CAB
 *   stdin: "AB\n"   → AB BA
 *   stdin: "A\n"    → A
 */
#include <stdio.h>
#include <string.h>

/* 交换两个字符 */
void swap(char *a, char *b) {
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

/* 全排列函数：输出字符串的所有排列 */
void permute(char *str, int l, int r) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 递归终止条件：l == r 时到达叶子节点，打印当前排列 */

    /* 枚举 i 从 l 到 r 的所有字符作为 l 位置的候选 */

    /* 把候选换到固定位置：swap(&str[l], &str[i]) */

    /* 递归处理 l+1 之后的子问题 */

    /* 回溯：把候选换回来（恢复原状态，继续尝试下一个候选） */
}

int main(void) {
    char str[256];
    fgets(str, sizeof(str), stdin);
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 调用 permute(str, 0, strlen(str) - 1) */
    return 0;
}
