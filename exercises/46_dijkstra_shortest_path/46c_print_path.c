// 46c_print_path.c — 回溯打印最短路径
//
// 任务: 在 Dijkstra 中记录前驱节点，回溯打印从源点到目标的路径
//
// 知识点: 路径回溯、前驱数组
//
// 测试图（5个节点）:
//   0→1:10, 0→3:5, 1→2:1, 1→3:2, 2→4:4, 3→1:3, 3→2:9, 3→4:2, 4→2:6
//
// 预期输出 (从0到4的最短路径):
//   0 -> 3 -> 4

#include <stdio.h>
#include <limits.h>

#define MAX_V 100
#define INF -1

int graph[MAX_V][MAX_V];
int dist[MAX_V];
int visited[MAX_V];
int prev_node[MAX_V];

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

// TODO: 实现 dijkstra 函数（含前驱记录）
// 功能: 同 46b，但增加 prev_node[] 记录
//   初始化 prev_node[i] = -1
//   松弛时: if (new_d < dist[v]) { dist[v] = new_d; prev_node[v] = u; }
//
// void dijkstra(int src, int n) { ... }

// TODO: 实现 print_path 函数
// 功能: 从 dst 沿 prev_node 回溯到 src，逆序打印路径
// 算法:
//   int path[MAX_V], len = 0;
//   for (int v = dst; v != -1; v = prev_node[v])
//       path[len++] = v;
//   for (int i = len-1; i >= 0; i--)
//       printf("%d%s", path[i], i > 0 ? " -> " : "\n");
//
// void print_path(int src, int dst) { ... }

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
    // TODO: 调用 print_path(0, 4) 打印从0到4的路径

    return 0;
}
