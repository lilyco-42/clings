/* 54_astar-pathfinding.c — A*寻路算法【标杆题】
 *
 * 在 6×5 网格上实现 A* 寻路，从 (0,0) 到 (5,4)，绕过障碍物。
 * A* 结合 Dijkstra 的实际代价 g(n) 和贪心的启发值 h(n)，
 * 用 f(n)=g(n)+h(n) 指导搜索。Manhattan 距离对网格地图是可采纳启发。
 *
 * 知识点：启发式搜索、Manhattan 距离、Open/Closed 表、路径回溯
 *
 * 验证：make test → 编译运行并 diff 比对 expected_output.txt
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ROWS 6
#define COLS 5
#define MAX_NODES (ROWS * COLS)

/* 地图：0=可通行，1=障碍物 */
static const int OBS[ROWS][COLS] = {
    {0, 0, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 1, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
};

/* ─── TODO 1: Node 结构体 + 全局数组 ─── */
#error TODO: Finish this exercise. Run "clings hint" for help.
/*
 * 定义结构体 Node { int r,c,g,h,f,parent; bool closed; }
 * 声明全局数组 Node nodes[MAX_NODES]; int node_count = 0;
 */

/* ─── TODO 2: Manhattan 启发函数 ─── */
#error TODO: Finish this exercise. Run "clings hint" for help.
/*
 * static int heuristic(int r, int c, int gr, int gc)
 * 返回 |r - gr| + |c - gc|
 */

/* ─── TODO 3: 查找或创建节点 ─── */
#error TODO: Finish this exercise. Run "clings hint" for help.
/*
 * static int find_or_create(int r, int c, int g, int parent, int gr, int gc)
 * 在 nodes[] 中查找坐标 (r,c)。若找到且新 g 更优则更新 g/f/parent；
 * 若未找到则创建新节点，计算 h 和 f，加入数组。返回节点索引。
 */

/* ─── TODO 4: 从 Open 表选 f 最小节点 ─── */
#error TODO: Finish this exercise. Run "clings hint" for help.
/*
 * static int pick_best(void)
 * 遍历 nodes[]，跳过 closed==true 的节点，返回 f 最小的索引。
 * f 相同时选 h 更小的。若无可选节点返回 -1。
 */

int main(void) {
    int sr = 0, sc = 0, gr = 5, gc = 4;
    printf("=== A* Pathfinding: 6x5 Grid ===\n");
    printf("Start: (%d,%d)  Goal: (%d,%d)\n", sr, sc, gr, gc);
    printf("Obstacles: (2,2) (3,2) (1,3) (3,3)\n\n");

    node_count = 0;
    find_or_create(sr, sc, 0, -1, gr, gc);
    int goal_idx = -1;

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* ═══ TODO 5: A* 主循环 ═══
     * while (1):
     *   从 Open 表选最优节点，若无可选节点则退出。
     *   标记节点为 closed，打印 Expand 信息。
     *   若到达终点则记录 goal_idx 并退出。
     *   扩展四个方向的邻居，检查边界和障碍物，
     *   用 find_or_create 将邻居加入/更新 Open 表。
     */

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* ═══ TODO 6: 路径重建 ═══
     * 若 goal_idx < 0 则打印 "No path found."。
     * 否则从 goal_idx 沿 parent 回溯到起点，存入 path[]，
     * 逆序输出路径坐标和长度。
     */
    return 0;
}
