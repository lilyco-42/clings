// 48d_gps_ncurses.c — ncurses 地图显示 (Linux-only)
//
// 任务: 使用 ncurses 库在终端显示城市图和最短路径
//       Linux 下编译: gcc -Wall -Wextra -std=c11 48d_gps_ncurses.c -o 48d -lncurses
//       非 Linux 环境下回退为文本输出
//
// 知识点: ncurses 库、条件编译、终端 UI
//
// 此练习使用 mode=make 测试，仅验证编译通过

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
    int pos_x[MAX_CITIES];  /* 显示 X 坐标 */
    int pos_y[MAX_CITIES];  /* 显示 Y 坐标 */
};

int shortest[MAX_CITIES];
int visited[MAX_CITIES];
int prev_node[MAX_CITIES];

// TODO: 实现 graph_init, graph_set_name, graph_add_edge（同 48a）

// TODO: 实现 graph_set_pos 函数
// 功能: 设置城市的显示坐标
// void graph_set_pos(struct city_graph *g, int id, int x, int y) { ... }

// TODO: 实现 dijkstra（同 48c，含 prev_node）

// TODO: 在 #ifdef __linux__ 中实现 display_map 函数
// 功能:
//   1. initscr(); cbreak(); noecho();
//   2. 设置颜色对（路径=绿色，普通=白色，起终点=黄色）
//   3. 标记路径上的节点
//   4. 遍历城市，用 mvprintw 在对应坐标显示城市名
//   5. 显示最短距离信息
//   6. getch(); endwin();

int main(void)
{
    struct city_graph g;
    // TODO: 初始化图、设置城市名和坐标、添加边

    // TODO: dijkstra(&g, 0)

#ifdef __linux__
    // TODO: display_map(&g, 0, 3);
#else
    printf("ncurses display requires Linux\n");
    // TODO: 打印文本路径作为 fallback
#endif

    (void)g;  /* 删除此行 */
    return 0;
}
