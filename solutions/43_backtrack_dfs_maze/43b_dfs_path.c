/*
 * Lesson 43b: DFS 找一条路径 — 参考答案
 */
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

int dr[] = {1, 0, -1, 0};
int dc[] = {0, 1, 0, -1};

struct point {
    int row, col;
};
struct point path[MAX_PATH];
int path_len = 0;

int dfs(int r, int c)
{
    path[path_len++] = (struct point){r, c};
    visited[r][c] = 1;

    if (r == N - 1 && c == N - 1) return 1;

    for (int d = 0; d < 4; d++) {
        int nr = r + dr[d], nc = c + dc[d];
        if (nr >= 0 && nr < N && nc >= 0 && nc < N
            && !visited[nr][nc] && maze[nr][nc] == 0) {
            if (dfs(nr, nc)) return 1;
        }
    }

    path_len--;
    return 0;
}

int main(void)
{
    if (dfs(0, 0)) {
        for (int i = 0; i < path_len; i++)
            printf("%s(%d,%d)", i > 0 ? " " : "", path[i].row, path[i].col);
        printf("\n");
    }
    return 0;
}
