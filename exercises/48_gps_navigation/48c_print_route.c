// 48c_print_route.c — 格式化打印导航路线
//
// 任务: 在 Dijkstra 中记录前驱节点，回溯打印路径及总距离
//       格式: "CityA -> CityB -> ... -> CityN (distance km)"
//
// 知识点: 路径回溯、字符串格式化
//
// 预期输出 (0→3, Beijing到Shenzhen):
//   Beijing -> Hangzhou -> Guangzhou -> Shenzhen (2750 km)

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
int prev_node[MAX_CITIES];

// TODO: 实现 graph_init, graph_set_name, graph_add_edge（同 48a）

// TODO: 实现 dijkstra 函数（含 prev_node 记录）
// 在松弛时同时记录: prev_node[v] = u
// void dijkstra(struct city_graph *g, int src) { ... }

// TODO: 实现 print_route 函数
// 功能:
//   1. 从 dst 沿 prev_node[] 回溯到 src，存入 path[] 数组
//   2. 逆序打印城市名，用 " -> " 连接
//   3. 末尾打印 " (distance km)\n"
//
// void print_route(struct city_graph *g, int dst) {
//     int path[MAX_CITIES], len = 0;
//     for (int v = dst; v != -1; v = prev_node[v])
//         path[len++] = v;
//     for (int i = len-1; i >= 0; i--) {
//         printf("%s", g->names[path[i]]);
//         if (i > 0) printf(" -> ");
//     }
//     printf(" (%d km)\n", shortest[dst]);
// }

int main(int argc, char *argv[])
{
    int src = 0, dst = 3;
    if (argc == 3) {
        src = atoi(argv[1]);
        dst = atoi(argv[2]);
    }

    struct city_graph g;
    // TODO: 初始化图（同 48a）
    // TODO: dijkstra(&g, src)
    // TODO: print_route(&g, dst)

    (void)g; (void)src; (void)dst;  /* 删除此行 */
    return 0;
}
