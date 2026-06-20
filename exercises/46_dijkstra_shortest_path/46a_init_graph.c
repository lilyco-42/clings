// 46a_init_graph.c — 初始化邻接矩阵有向图
//
// 任务: 用邻接矩阵表示有向加权图
//       初始化图后打印第 0 行
//
// 知识点: 邻接矩阵、图的表示
//
// 测试图（5个节点）:
//   0→1:10, 0→3:5, 1→2:1, 1→3:2, 2→4:4, 3→1:3, 3→2:9, 3→4:2, 4→2:6
//
// 预期输出:
//   0 10 -1 5 -1

#include <stdio.h>

#define MAX_V 100
#define INF -1

int graph[MAX_V][MAX_V];
int num_vertices;

// TODO: 实现 init_graph 函数
// 功能: 将 graph 全部初始化为 INF，对角线 graph[i][i] = 0
// void init_graph(int n) { ... }

// TODO: 实现 add_edge 函数
// 功能: 设置有向边 graph[u][v] = weight
// void add_edge(int u, int v, int weight) { ... }

// TODO: 实现 print_row 函数
// 功能: 打印邻接矩阵的第 row 行，用空格分隔
// void print_row(int row, int n) { ... }

int main(void)
{
    num_vertices = 5;

    // TODO: 调用 init_graph(5)
    // TODO: 添加边:
    //   add_edge(0, 1, 10); add_edge(0, 3, 5);
    //   add_edge(1, 2, 1);  add_edge(1, 3, 2);
    //   add_edge(2, 4, 4);
    //   add_edge(3, 1, 3);  add_edge(3, 2, 9); add_edge(3, 4, 2);
    //   add_edge(4, 2, 6);
    // TODO: 调用 print_row(0, 5)

    return 0;
}
