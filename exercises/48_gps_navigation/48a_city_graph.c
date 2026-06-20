// 48a_city_graph.c — 城市图数据结构
//
// 任务: 定义城市图的邻接矩阵数据结构，初始化并添加边
//       打印城市数量和边数
//
// 知识点: 结构体设计、邻接矩阵、无向图
//
// 城市: Beijing(0), Shanghai(1), Guangzhou(2), Shenzhen(3), Hangzhou(4)
// 道路: Beijing-Shanghai:1200, Beijing-Hangzhou:1400,
//       Shanghai-Hangzhou:200, Shanghai-Guangzhou:1500,
//       Hangzhou-Guangzhou:1200, Guangzhou-Shenzhen:150
//
// 预期输出:
//   cities: 5
//   edges: 6

#include <stdio.h>
#include <string.h>

#define MAX_CITIES 100
#define INF -1

struct city_graph {
    int num_cities;
    int num_edges;
    int dist[MAX_CITIES][MAX_CITIES];  /* 邻接矩阵，INF 表示无边 */
    char names[MAX_CITIES][32];        /* 城市名称 */
};

// TODO: 实现 graph_init 函数
// 功能: 设置 num_cities=n, num_edges=0
//       dist[i][j] = 0 (i==j) 或 INF (i!=j)
// void graph_init(struct city_graph *g, int n) { ... }

// TODO: 实现 graph_set_name 函数
// 功能: 复制城市名到 g->names[id]
// void graph_set_name(struct city_graph *g, int id, const char *name) { ... }

// TODO: 实现 graph_add_edge 函数
// 功能: 设置 dist[u][v] = dist[v][u] = weight（无向图），num_edges++
// void graph_add_edge(struct city_graph *g, int u, int v, int weight) { ... }

int main(void)
{
    struct city_graph g;

    // TODO: 初始化 5 个城市的图
    // TODO: 设置城市名
    // TODO: 添加 6 条边
    // TODO: 打印 printf("cities: %d\nedges: %d\n", g.num_cities, g.num_edges);

    return 0;
}
