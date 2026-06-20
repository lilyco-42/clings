/*
 * Lesson 46c: 回溯打印最短路径 — 参考答案
 */
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

void dijkstra(int src, int n)
{
    for (int i = 0; i < n; i++) {
        dist[i] = INF;
        visited[i] = 0;
        prev_node[i] = -1;
    }
    dist[src] = 0;

    for (int count = 0; count < n; count++) {
        int u = -1, min_d = INT_MAX;
        for (int v = 0; v < n; v++) {
            if (!visited[v] && dist[v] != INF && dist[v] < min_d) {
                min_d = dist[v];
                u = v;
            }
        }

        if (u == -1) break;
        visited[u] = 1;

        for (int v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] != INF && graph[u][v] != 0) {
                int new_d = dist[u] + graph[u][v];
                if (dist[v] == INF || new_d < dist[v]) {
                    dist[v] = new_d;
                    prev_node[v] = u;
                }
            }
        }
    }
}

void print_path(int dst)
{
    int path[MAX_V], len = 0;
    for (int v = dst; v != -1; v = prev_node[v])
        path[len++] = v;
    for (int i = len - 1; i >= 0; i--)
        printf("%d%s", path[i], i > 0 ? " -> " : "\n");
}

int main(void)
{
    int n = 5;
    init_graph(n);

    add_edge(0, 1, 10); add_edge(0, 3, 5);
    add_edge(1, 2, 1);  add_edge(1, 3, 2);
    add_edge(2, 4, 4);
    add_edge(3, 1, 3);  add_edge(3, 2, 9); add_edge(3, 4, 2);
    add_edge(4, 2, 6);

    dijkstra(0, n);
    print_path(4);

    return 0;
}
