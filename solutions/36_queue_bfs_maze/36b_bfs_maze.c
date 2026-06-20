/*
 * Lesson 36b: BFS 解迷宫最短路径 — 参考答案
 */
#include <stdio.h>

#define N 5
#define MAX_SIZE 100

/* 迷宫: 0=通路, 1=墙壁 */
int maze[N][N] = {
    {0, 0, 1, 1, 1},
    {0, 1, 1, 1, 1},
    {0, 0, 0, 0, 0},
    {1, 1, 1, 1, 0},
    {1, 1, 1, 1, 0}
};

struct point {
    int row, col;
};

struct point queue[MAX_SIZE];
int front = 0, rear = 0;

void enqueue(struct point p) { queue[rear++] = p; }
struct point dequeue(void) { return queue[front++]; }
int is_empty(void) { return front == rear; }

int prev_row[N][N];
int prev_col[N][N];
int visited[N][N];

/* 四个方向: 下、右、上、左 */
int dr[] = {1, 0, -1, 0};
int dc[] = {0, 1, 0, -1};

int bfs(void)
{
    visited[0][0] = 1;
    enqueue((struct point){0, 0});

    while (!is_empty()) {
        struct point cur = dequeue();
        if (cur.row == N - 1 && cur.col == N - 1)
            return 1;
        for (int d = 0; d < 4; d++) {
            int nr = cur.row + dr[d];
            int nc = cur.col + dc[d];
            if (nr >= 0 && nr < N && nc >= 0 && nc < N
                && !visited[nr][nc] && maze[nr][nc] == 0) {
                visited[nr][nc] = 1;
                prev_row[nr][nc] = cur.row;
                prev_col[nr][nc] = cur.col;
                enqueue((struct point){nr, nc});
            }
        }
    }
    return 0;
}

void print_path(void)
{
    struct point path[MAX_SIZE];
    int len = 0;
    int r = N - 1, c = N - 1;

    while (r != 0 || c != 0) {
        path[len++] = (struct point){r, c};
        int pr = prev_row[r][c];
        int pc = prev_col[r][c];
        r = pr;
        c = pc;
    }
    path[len++] = (struct point){0, 0};

    for (int i = len - 1; i >= 0; i--) {
        if (i < len - 1) printf(" ");
        printf("(%d, %d)", path[i].row, path[i].col);
    }
    printf("\n");
}

int main(void)
{
    if (bfs())
        print_path();

    return 0;
}
