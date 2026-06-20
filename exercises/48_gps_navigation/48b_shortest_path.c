// 48b_shortest_path.c — Dijkstra 城市最短路径
//
// 任务: 用 Dijkstra 算法计算两城市间的最短距离
//       默认计算 Beijing(0) 到 Shenzhen(3) 的最短距离
//       也可从命令行参数指定: argv[1]=src, argv[2]=dst
//
// 知识点: Dijkstra 算法、无向加权图
//
// 预期输出 (Beijing→Shenzhen):
//   2750

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define MAX_CITIES 100
#define INF -1

struct city_graph {
    int num_cities;
    int num_edges;
    int dist[MAX_CITIES][MAX_CITIES];
    char names[MAX_CITIES][32];
};

int shortest[MAX_CITIES];
int visited[MAX_CITIES];

// TODO: 实现 graph_init, graph_set_name, graph_add_edge（同 48a）

// TODO: 实现 dijkstra 函数
// 功能: 从 src 出发，计算到所有城市的最短距离，存入 shortest[]
// 注意: 无向图，所以 dist[u][v] 和 dist[v][u] 都可能有值
//
// void dijkstra(struct city_graph *g, int src) {
//     初始化 shortest[] = INF, visited[] = 0, shortest[src] = 0
//     重复 n 次:
//       找未访问中 shortest 最小的节点 u
//       标记 u 已访问
//       松弛 u 的所有邻居
// }

int main(int argc, char *argv[])
{
    int src = 0, dst = 3;
    if (argc == 3) {
        src = atoi(argv[1]);
        dst = atoi(argv[2]);
    }

    struct city_graph g;
    // TODO: 初始化图（同 48a 的城市和边）
    // TODO: 调用 dijkstra(&g, src)
    // TODO: printf("%d\n", shortest[dst]);

    (void)g; (void)src; (void)dst;  /* 删除此行 */
    return 0;
}
