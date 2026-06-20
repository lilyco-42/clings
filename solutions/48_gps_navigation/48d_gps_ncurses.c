/*
 * Lesson 48d: ncurses 地图显示 — 参考答案
 *
 * 使用 ncurses 显示城市图和最短路径（Linux-only）
 * 编译: gcc -Wall -Wextra -std=c11 48d_gps_ncurses.c -o 48d -lncurses
 *
 * 此练习仅测试编译通过（mode=make），不检查运行时输出
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#ifdef __linux__
#include <ncurses.h>
#endif

#define MAX_CITIES 100
#define INF -1

struct city_graph {
    int num_cities;
    int num_edges;
    int dist[MAX_CITIES][MAX_CITIES];
    char names[MAX_CITIES][32];
    int pos_x[MAX_CITIES];  /* 显示坐标 */
    int pos_y[MAX_CITIES];
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

void graph_set_pos(struct city_graph *g, int id, int x, int y)
{
    g->pos_x[id] = x;
    g->pos_y[id] = y;
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

#ifdef __linux__
void display_map(struct city_graph *g, int src, int dst)
{
    initscr();
    cbreak();
    noecho();

    if (has_colors()) {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);  /* 路径 */
        init_pair(2, COLOR_WHITE, COLOR_BLACK);  /* 普通城市 */
        init_pair(3, COLOR_YELLOW, COLOR_BLACK); /* 起终点 */
    }

    /* 标记路径上的节点 */
    int on_path[MAX_CITIES] = {0};
    for (int v = dst; v != -1; v = prev_node[v])
        on_path[v] = 1;

    /* 绘制城市 */
    for (int i = 0; i < g->num_cities; i++) {
        if (on_path[i]) {
            attron(COLOR_PAIR(i == src || i == dst ? 3 : 1));
            mvprintw(g->pos_y[i], g->pos_x[i], "[%s]", g->names[i]);
            attroff(COLOR_PAIR(i == src || i == dst ? 3 : 1));
        } else {
            attron(COLOR_PAIR(2));
            mvprintw(g->pos_y[i], g->pos_x[i], " %s ", g->names[i]);
            attroff(COLOR_PAIR(2));
        }
    }

    mvprintw(22, 0, "Shortest: %d km | Press any key to exit", shortest[dst]);
    refresh();
    getch();
    endwin();
}
#endif

int main(void)
{
    struct city_graph g;
    graph_init(&g, 5);

    graph_set_name(&g, 0, "Beijing");
    graph_set_name(&g, 1, "Shanghai");
    graph_set_name(&g, 2, "Guangzhou");
    graph_set_name(&g, 3, "Shenzhen");
    graph_set_name(&g, 4, "Hangzhou");

    graph_set_pos(&g, 0, 30, 2);
    graph_set_pos(&g, 1, 50, 8);
    graph_set_pos(&g, 2, 40, 16);
    graph_set_pos(&g, 3, 45, 18);
    graph_set_pos(&g, 4, 45, 10);

    graph_add_edge(&g, 0, 1, 1200);
    graph_add_edge(&g, 0, 4, 1400);
    graph_add_edge(&g, 1, 4, 200);
    graph_add_edge(&g, 1, 2, 1500);
    graph_add_edge(&g, 4, 2, 1200);
    graph_add_edge(&g, 2, 3, 150);

    dijkstra(&g, 0);

#ifdef __linux__
    display_map(&g, 0, 3);
#else
    printf("ncurses display requires Linux\n");
    printf("Beijing -> Hangzhou -> Guangzhou -> Shenzhen (%d km)\n", shortest[3]);
#endif

    return 0;
}
