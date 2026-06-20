// 43b_dfs_path.c — DFS 找一条路径
//
// 任务: 使用 DFS 在 5x5 迷宫中找到从 (0,0) 到 (4,4) 的一条路径
//       方向优先级: 下、右、上、左
//
// 知识点: DFS 路径搜索、路径记录、提前返回
//
// 迷宫 (0=通路, 1=墙壁):
//   0 1 0 0 0
//   0 1 0 1 0
//   0 0 0 0 0
//   0 1 1 1 0
//   0 0 0 1 0
//
// 预期输出:
//   (0,0) (1,0) (2,0) (2,1) (2,2) (2,3) (2,4) (3,4) (4,4)

#include <stdio.h>

#define N 5
#define MAX_PATH 100

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

/* 路径记录 */
struct point {
    int row, col;
};
struct point path[MAX_PATH];
int path_len = 0;

// TODO: 实现 DFS 路径搜索函数
// 功能: 从 (r, c) 开始 DFS，找到到 (N-1, N-1) 的路径
//   1. 将 (r, c) 加入路径: path[path_len++] = {r, c}
//   2. 标记 visited[r][c] = 1
//   3. 如果 (r, c) == (N-1, N-1)，返回 1（找到）
//   4. 按方向顺序尝试邻居:
//      - 如果邻居有效且未访问且不是墙:
//        - 递归调用 dfs(nr, nc)
//        - 如果返回 1，直接返回 1（传递成功）
//   5. 回溯: path_len--（从路径中移除当前点）
//   6. 返回 0（此路不通）
// int dfs(int r, int c) { ... }

int main(void)
{
    // TODO: 调用 dfs(0, 0)
    // TODO: 如果找到路径，打印路径
    //   for (int i = 0; i < path_len; i++)
    //       printf("%s(%d,%d)", i > 0 ? " " : "", path[i].row, path[i].col);
    //   printf("\n");

    return 0;
}
