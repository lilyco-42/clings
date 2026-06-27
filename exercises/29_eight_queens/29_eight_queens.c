/* 29_eight_queens.c — 八皇后问题：回溯 + 剪枝 【标杆题】
 *
 * 任务: 1. 实现 is_safe() — 冲突检测
 *       2. 实现 solve()  — 回溯求解
 *       输出 92 个解的个数
 *
 * 背景: 在 8×8 棋盘上放置 8 个皇后，任意两个不能在同一行/列/对角线
 *       用一维数组 col[8] 降维：col[row] = 第 row 行的皇后所在列号
 *
 * 算法: 逐行放置 + 冲突检测 + 回溯
 *       1. 用一维数组 col[8]: col[row] = 皇后在第 row 行放置的列号
 *       2. 对第 row 行，枚举 0~7 列：
 *          a. 检查与之前行 (0..row-1) 是否冲突
 *          b. 不冲突则放置，递归处理 row+1 行
 *          c. 回溯（无需显式撤销，因为会覆盖）
 *       3. row == 8 时找到一个解，计数 +1
 *
 * 对角线冲突判断:
 *       两个皇后 (i, col[i]) 和 (j, col[j]) 在同一对角线上
 *       当且仅当 |i - j| == |col[i] - col[j]|
 *       （斜率绝对值为 1，即 45° 方向）
 *
 * 知识点: 回溯剪枝、对角线判断、一维数组降维、abs()
 *
 * 验证: (无输入) → 92
 */
#include <stdio.h>
#include <stdlib.h> /* abs() */

int count = 0;
int col[8]; /* col[row] = 皇后在第 row 行所在的列 */

/* 判断在第 row 行第 c 列放置皇后是否安全
 * 需要检测：与前面 0..row-1 行是否有同列或同对角线冲突 */
int is_safe(int row, int c) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 遍历已经放置好的第 0 到 row-1 行 */

    /* 检测 1：同列冲突 — 新皇后和已有皇后在同一列 */

    /* 检测 2：对角线冲突 — 行列差的绝对值相等 */

    /* 都通过则安全，返回 1 */
}

/* 回溯求解：回溯 + 剪枝 逐行放置皇后 */
void solve(int row) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 终止条件：所有 8 行都放好了 → 找到一个解，计数 +1 */

    /* 枚举当前行的每一列 c ∈ [0, 7] */

    /* 如果 (row, c) 安全 → 放置，递归处理下一行 */
}

int main(void) {
    count = 0;
    solve(0);
    printf("%d\n", count);
    return 0;
}
