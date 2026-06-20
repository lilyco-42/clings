// 43a_dfs_visit.c — DFS 标记可达格子
//
// 任务: 使用 DFS 从 (0,0) 出发，标记 5x5 迷宫中所有可达的格子，输出可达格子数
//
// 知识点: DFS 递归遍历、visited 数组、四方向搜索
//
// 迷宫 (0=通路, 1=墙壁):
//   0 1 0 0 0
//   0 1 0 1 0
//   0 0 0 0 0
//   0 1 1 1 0
//   0 0 0 1 0
//
// 预期输出:
//   reachable: 18

#include <stdio.h>

#define N 5

/* 迷宫定义: 0=通路, 1=墙壁 */
int maze[N][N] = {
    {0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0}
};

int visited[N][N];

/* 四个方向: 下、右、上、左 */
int dr[] = {1, 0, -1, 0};
int dc[] = {0, 1, 0, -1};

// TODO: 实现 DFS 递归函数
// 功能: 从 (r, c) 开始深度优先搜索，标记所有可达格子
//   1. 标记 visited[r][c] = 1
//   2. 尝试四个方向:
//      - 计算 nr = r + dr[d], nc = c + dc[d]
//      - 检查边界（0<=nr<N, 0<=nc<N）
//      - 检查未访问 && 不是墙
//      - 递归调用 dfs(nr, nc)
// void dfs(int r, int c) { ... }

int main(void)
{
    // TODO: 调用 dfs(0, 0)
    // TODO: 遍历 visited[][] 统计值为 1 的格子数
    // TODO: printf("reachable: %d\n", count);

    return 0;
}
