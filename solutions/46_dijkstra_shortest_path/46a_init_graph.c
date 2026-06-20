/*
 * Lesson 46a: 初始化邻接矩阵有向图 — 参考答案
 */
#include <stdio.h>

#define MAX_V 100
#define INF -1

int graph[MAX_V][MAX_V];
int num_vertices;

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

void print_row(int row, int n)
{
    for (int j = 0; j < n; j++)
        printf("%d%c", graph[row][j], j < n - 1 ? ' ' : '\n');
}

int main(void)
{
    num_vertices = 5;

    init_graph(5);

    add_edge(0, 1, 10); add_edge(0, 3, 5);
    add_edge(1, 2, 1);  add_edge(1, 3, 2);
    add_edge(2, 4, 4);
    add_edge(3, 1, 3);  add_edge(3, 2, 9); add_edge(3, 4, 2);
    add_edge(4, 2, 6);

    print_row(0, 5);

    return 0;
}
