/*
 * Lesson 36b: BFS 标记可达格子 — 参考答案
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

int visited[N][N];

/* 四个方向: 上、下、左、右 */
int dr[] = {-1, 1, 0, 0};
int dc[] = {0, 0, -1, 1};

void bfs_visit(void)
{
    visited[0][0] = 1;
    enqueue((struct point){0, 0});

    while (!is_empty()) {
        struct point cur = dequeue();
        for (int d = 0; d < 4; d++) {
            int nr = cur.row + dr[d];
            int nc = cur.col + dc[d];
            if (nr >= 0 && nr < N && nc >= 0 && nc < N
                && !visited[nr][nc] && maze[nr][nc] == 0) {
                visited[nr][nc] = 1;
                enqueue((struct point){nr, nc});
            }
        }
    }
}

int main(void)
{
    bfs_visit();

    int count = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (visited[i][j])
                count++;

    printf("reachable: %d\n", count);

    return 0;
}
