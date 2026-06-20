// 46b_dijkstra.c — Dijkstra 最短路径算法
//
// 任务: 实现 Dijkstra 算法，从节点 0 出发计算到所有节点的最短距离
//
// 知识点: 贪心算法、最短路径、松弛操作
//
// 测试图（5个节点）:
//   0→1:10, 0→3:5, 1→2:1, 1→3:2, 2→4:4, 3→1:3, 3→2:9, 3→4:2, 4→2:6
//
// 预期输出 (从节点0出发的最短距离):
//   0 8 9 5 7

#include <stdio.h>
#include <limits.h>

#define MAX_V 100
#define INF -1

int graph[MAX_V][MAX_V];
int dist[MAX_V];
int visited[MAX_V];

void init_graph(int n)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            graph[i][j] = (i == j) ? 0 : INF;
}

void add_edge(int u, int v, int weight)
{
    graph[u][v] = weight;
}

// TODO: 实现 dijkstra 函数
// 功能: 从 src 出发，计算到所有节点的最短距离，结果存入 dist[]
// 算法:
//   1. 初始化 dist[] 全为 INF，dist[src] = 0，visited[] 全为 0
//   2. 重复 n 次:
//      a. 从未访问节点中选 dist 最小的节点 u
//      b. 标记 u 为已访问
//      c. 对 u 的所有邻居 v，尝试松弛: if (dist[u] + graph[u][v] < dist[v]) ...
//
// void dijkstra(int src, int n) { ... }

int main(void)
{
    int n = 5;
    init_graph(n);

    add_edge(0, 1, 10); add_edge(0, 3, 5);
    add_edge(1, 2, 1);  add_edge(1, 3, 2);
    add_edge(2, 4, 4);
    add_edge(3, 1, 3);  add_edge(3, 2, 9); add_edge(3, 4, 2);
    add_edge(4, 2, 6);

    // TODO: 调用 dijkstra(0, n)
    // TODO: 打印 dist[0..n-1]，空格分隔，换行结尾
    //   for (int i = 0; i < n; i++)
    //       printf("%d%c", dist[i], i < n-1 ? ' ' : '\n');

    return 0;
}
