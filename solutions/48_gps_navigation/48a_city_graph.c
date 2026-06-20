/*
 * Lesson 48a: 城市图数据结构 — 参考答案
 *
 * 使用邻接矩阵表示无向加权图（城市间道路距离）
 * 5个城市: Beijing(0), Shanghai(1), Guangzhou(2), Shenzhen(3), Hangzhou(4)
 */
#include <stdio.h>
#include <string.h>

#define MAX_CITIES 100
#define INF -1

struct city_graph {
    int num_cities;
    int num_edges;
    int dist[MAX_CITIES][MAX_CITIES];  /* 邻接矩阵 */
    char names[MAX_CITIES][32];        /* 城市名 */
};

void graph_init(struct city_graph *g, int n)
{
    g->num_cities = n;
    g->num_edges = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            g->dist[i][j] = (i == j) ? 0 : INF;
}

void graph_set_name(struct city_graph *g, int id, const char *name)
{
    strncpy(g->names[id], name, 31);
    g->names[id][31] = '\0';
}

void graph_add_edge(struct city_graph *g, int u, int v, int weight)
{
    g->dist[u][v] = weight;
    g->dist[v][u] = weight;  /* 无向图 */
    g->num_edges++;
}

int main(void)
{
    struct city_graph g;
    graph_init(&g, 5);

    graph_set_name(&g, 0, "Beijing");
    graph_set_name(&g, 1, "Shanghai");
    graph_set_name(&g, 2, "Guangzhou");
    graph_set_name(&g, 3, "Shenzhen");
    graph_set_name(&g, 4, "Hangzhou");

    graph_add_edge(&g, 0, 1, 1200);  /* Beijing - Shanghai */
    graph_add_edge(&g, 0, 4, 1400);  /* Beijing - Hangzhou */
    graph_add_edge(&g, 1, 4, 200);   /* Shanghai - Hangzhou */
    graph_add_edge(&g, 1, 2, 1500);  /* Shanghai - Guangzhou */
    graph_add_edge(&g, 4, 2, 1200);  /* Hangzhou - Guangzhou */
    graph_add_edge(&g, 2, 3, 150);   /* Guangzhou - Shenzhen */

    printf("cities: %d\nedges: %d\n", g.num_cities, g.num_edges);

    return 0;
}
