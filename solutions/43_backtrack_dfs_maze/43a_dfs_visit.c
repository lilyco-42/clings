/*
 * Lesson 43a: DFS 标记可达格子 — 参考答案
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

int dr[] = {1, 0, -1, 0};
int dc[] = {0, 1, 0, -1};

void dfs(int r, int c)
{
    visited[r][c] = 1;
    for (int d = 0; d < 4; d++) {
        int nr = r + dr[d], nc = c + dc[d];
        if (nr >= 0 && nr < N && nc >= 0 && nc < N
            && !visited[nr][nc] && maze[nr][nc] == 0) {
            dfs(nr, nc);
        }
    }
}

int main(void)
{
    dfs(0, 0);

    int count = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (visited[i][j]) count++;

    printf("reachable: %d\n", count);
    return 0;
}
