// 36b_bfs_maze.c — BFS 解决迷宫最短路径
//
// 任务: 使用队列实现 BFS，找到 5x5 迷宫从 (0,0) 到 (4,4) 的最短路径
//       并按顺序输出路径坐标
//
// 知识点: BFS 算法、队列应用、路径回溯
//
// 预期输出:
//   (0, 0) (1, 0) (2, 0) (2, 1) (2, 2) (2, 3) (2, 4) (3, 4) (4, 4)
//
// 迷宫 (0=通路, 1=墙壁):
//   0 0 1 1 1
//   0 1 1 1 1
//   0 0 0 0 0
//   1 1 1 1 0
//   1 1 1 1 0

#include <stdio.h>

#define N 5
#define MAX_SIZE 100

// 迷宫定义: 0=通路, 1=墙壁
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

// 记录每个点的前驱，用于回溯路径
int prev_row[N][N];
int prev_col[N][N];
int visited[N][N];

// 四个方向: 下、右、上、左
int dr[] = {1, 0, -1, 0};
int dc[] = {0, 1, 0, -1};

// TODO: 实现 bfs 函数
// 功能: 从 (0,0) 开始 BFS 搜索到 (N-1, N-1)
//   1. 标记起点已访问，enqueue 起点
//   2. 循环直到队列空:
//      a. dequeue 一个点 cur
//      b. 如果 cur 是终点，返回 1
//      c. 尝试四个方向的邻居:
//         - 检查边界、未访问、不是墙
//         - 标记已访问，记录前驱，enqueue
//   3. 找不到路径返回 0
// int bfs(void) { ... }

// TODO: 实现 print_path 函数
// 功能: 从终点回溯到起点，收集路径，然后正序打印
//   用数组存储路径点，从终点往回走（通过 prev_row/prev_col），
//   然后倒序打印: "(row, col) "
// void print_path(void) { ... }

int main(void)
{
    // TODO: 初始化 visited/prev 数组
    // TODO: 调用 bfs()
    // TODO: 如果找到路径，调用 print_path()

    return 0;
}
