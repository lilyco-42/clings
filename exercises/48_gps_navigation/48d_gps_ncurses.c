// 48d_gps_ncurses.c — ncurses 地图显示 + 文本双模式
//
// 任务: 使用 ncurses 库在终端显示城市图和最短路径。
//       程序自动检测运行环境:
//       - 终端(isatty): ncurses 交互式地图 UI
//       - 非终端(CI/管道): 输出文本格式的路径信息
//
// 知识点: ncurses(initscr/mvprintw/getch/endwin)、isatty、条件分支
//
// 编译: gcc -Wall -Wextra -std=c11 48d_gps_ncurses.c -o 48d_gps_ncurses -lncurses
//
// 预期输出（非终端模式）:
//   === GPS Navigation ===
//   Cities: 5, Edges: 6
//   Route: Beijing -> Hangzhou -> Guangzhou -> Shenzhen (2750 km)
//   Distance: 2750 km

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <ncurses.h>

#define MAX_CITIES 100
#define INF -1

struct city_graph {
    int num_cities;
    int num_edges;
    int dist[MAX_CITIES][MAX_CITIES];
    char names[MAX_CITIES][32];
    int pos_x[MAX_CITIES];
    int pos_y[MAX_CITIES];
};

static int shortest[MAX_CITIES];
static int visited[MAX_CITIES];
static int prev_node[MAX_CITIES];

// TODO: 实现 graph_init, graph_set_name, graph_set_pos, graph_add_edge（同 48a）

// TODO: 实现 dijkstra（同 48b/48c，需要填充 prev_node）

// TODO: 实现 print_path_text(g, src, dst)
//       从 prev_node 回溯路径，打印: "Beijing -> Hangzhou -> ... (N km)\n"

// TODO: 实现 display_text(g, src, dst)
//       非终端模式输出:
//         printf("=== GPS Navigation ===\n");
//         printf("Cities: %d, Edges: %d\n", ...);
//         printf("Route: "); print_path_text(g, src, dst);
//         printf("Distance: %d km\n", shortest[dst]);

// TODO: 实现 display_ncurses(g, src, dst)
//       终端模式:
//       1. initscr(); cbreak(); noecho();
//       2. start_color(); init_pair(1, COLOR_GREEN, COLOR_BLACK); ...
//       3. 标记路径节点: for (v = dst; v != -1; v = prev_node[v]) on_path[v] = 1
//       4. 遍历城市: mvprintw(pos_y, pos_x, ...) 路径用绿色，起终点用黄色
//       5. mvprintw(22, 0, "Shortest: ... | Press any key to exit");
//       6. refresh(); getch(); endwin();

int main(void)
{
    struct city_graph g;
    // TODO: 初始化图（5 个城市，6 条边，同 48a/48b/48c）

    // TODO: dijkstra(&g, 0)

    // TODO: 根据 isatty(STDOUT_FILENO) 选择显示模式
    //   if (isatty(STDOUT_FILENO))
    //       display_ncurses(&g, 0, 3);
    //   else
    //       display_text(&g, 0, 3);

    (void)g;
    return 0;
}
