/*
 * Lesson 48c: 格式化打印导航路线 — 参考答案
 *
 * 输出格式: "CityA -> CityB -> CityC (distance km)"
 */
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
    g->dist[v][u] = weight;
    g->num_edges++;
}

int shortest[MAX_CITIES];
int visited[MAX_CITIES];
int prev_node[MAX_CITIES];

void dijkstra(struct city_graph *g, int src)
{
    int n = g->num_cities;
    for (int i = 0; i < n; i++) {
        shortest[i] = INF;
        visited[i] = 0;
        prev_node[i] = -1;
    }
    shortest[src] = 0;

    for (int count = 0; count < n; count++) {
        int u = -1, min_d = INT_MAX;
        for (int v = 0; v < n; v++) {
            if (!visited[v] && shortest[v] != INF && shortest[v] < min_d) {
                min_d = shortest[v];
                u = v;
            }
        }
        if (u == -1) break;
        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (!visited[v] && g->dist[u][v] != INF && g->dist[u][v] != 0) {
                int new_d = shortest[u] + g->dist[u][v];
                if (shortest[v] == INF || new_d < shortest[v]) {
                    shortest[v] = new_d;
                    prev_node[v] = u;
                }
            }
        }
    }
}

void print_route(struct city_graph *g, int dst)
{
    int path[MAX_CITIES], len = 0;
    for (int v = dst; v != -1; v = prev_node[v])
        path[len++] = v;

    for (int i = len - 1; i >= 0; i--) {
        printf("%s", g->names[path[i]]);
        if (i > 0) printf(" -> ");
    }
    printf(" (%d km)\n", shortest[dst]);
}

int main(int argc, char *argv[])
{
    int src = 0, dst = 3;
    if (argc == 3) {
        src = atoi(argv[1]);
        dst = atoi(argv[2]);
    }

    struct city_graph g;
    graph_init(&g, 5);

    graph_set_name(&g, 0, "Beijing");
    graph_set_name(&g, 1, "Shanghai");
    graph_set_name(&g, 2, "Guangzhou");
    graph_set_name(&g, 3, "Shenzhen");
    graph_set_name(&g, 4, "Hangzhou");

    graph_add_edge(&g, 0, 1, 1200);
    graph_add_edge(&g, 0, 4, 1400);
    graph_add_edge(&g, 1, 4, 200);
    graph_add_edge(&g, 1, 2, 1500);
    graph_add_edge(&g, 4, 2, 1200);
    graph_add_edge(&g, 2, 3, 150);

    dijkstra(&g, src);
    print_route(&g, dst);

    return 0;
}
