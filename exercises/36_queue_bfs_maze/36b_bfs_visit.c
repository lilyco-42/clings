// 36b_bfs_visit.c — BFS 标记可达格子
//
// 任务: 使用 BFS 从 (0,0) 出发，标记 5x5 迷宫中所有可达的格子，输出可达格子数
//
// 知识点: BFS 遍历、visited 数组、四方向搜索
//
// 迷宫 (0=通路, 1=墙壁):
//   0 0 1 1 1
//   0 1 1 1 1
//   0 0 0 0 0
//   1 1 1 1 0
//   1 1 1 1 0
//
// 预期输出:
//   reachable: 10

#include <stdio.h>

#define N 5
#define MAX_SIZE 100

/* 迷宫定义: 0=通路, 1=墙壁 */
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

// TODO: 实现 bfs_visit 函数
// 功能: 从 (0,0) 开始 BFS，标记所有可达的格子
//   1. 标记 (0,0) 已访问，入队
//   2. 循环直到队列空:
//      a. 出队一个点 cur
//      b. 尝试四个方向的邻居:
//         - 检查边界（0<=nr<N, 0<=nc<N）
//         - 检查未访问 && 不是墙（maze[nr][nc]==0）
//         - 标记已访问，入队
// void bfs_visit(void) { ... }

int main(void)
{
    // TODO: 调用 bfs_visit()
    // TODO: 遍历 visited[][] 统计值为 1 的格子数
    // TODO: printf("reachable: %d\n", count);

    return 0;
}
