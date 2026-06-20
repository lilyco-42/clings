/*
 * Lesson 43c: DFS + 回溯找所有路径 — 参考答案
 */
#include <stdio.h>

#define N 5

int maze[N][N] = {
    {0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0},
    {0, 0, 0, 1, 0}
};

int visited[N][N];
int count = 0;

int dr[] = {1, 0, -1, 0};
int dc[] = {0, 1, 0, -1};

void dfs(int r, int c)
{
    visited[r][c] = 1;

    if (r == N - 1 && c == N - 1) {
        count++;
    } else {
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d], nc = c + dc[d];
            if (nr >= 0 && nr < N && nc >= 0 && nc < N
                && !visited[nr][nc] && maze[nr][nc] == 0) {
                dfs(nr, nc);
            }
        }
    }

    visited[r][c] = 0;
}

int main(void)
{
    dfs(0, 0);
    printf("paths: %d\n", count);
    return 0;
}
