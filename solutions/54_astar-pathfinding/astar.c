/* 54_astar-pathfinding — A* Pathfinding on 6x5 Grid (solution) */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 6
#define COLS 5
#define MAX_NODES (ROWS * COLS)

/* ─── Grid and obstacles ─── */
static const int OBS[ROWS][COLS] = {
    {0, 0, 0, 0, 0}, {0, 0, 0, 1, 0}, {0, 0, 1, 0, 0}, {0, 0, 1, 1, 0}, {0, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
};

/* ─── Node structure ─── */
typedef struct {
    int r, c;    /* coordinates */
    int g;       /* cost from start */
    int h;       /* heuristic to goal */
    int f;       /* g + h */
    int parent;  /* index of parent node (-1 for start) */
    bool closed; /* already expanded */
} Node;

static Node nodes[MAX_NODES];
static int node_count = 0;

/* Manhattan distance heuristic */
static int heuristic(int r, int c, int gr, int gc) {
    int dr = abs(r - gr);
    int dc = abs(c - gc);
    return dr + dc;
}

/* Find or create a node; return its index.
 * If already exists and new g is better, update it.
 * Otherwise create a new node with h and f computed. */
static int find_or_create(int r, int c, int g, int parent, int gr, int gc) {
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].r == r && nodes[i].c == c) {
            /* Already exists; update if better g */
            if (g < nodes[i].g) {
                nodes[i].g = g;
                nodes[i].f = g + nodes[i].h;
                nodes[i].parent = parent;
            }
            return i;
        }
    }
    /* Create new node */
    Node n;
    n.r = r;
    n.c = c;
    n.g = g;
    n.h = heuristic(r, c, gr, gc);
    n.f = n.g + n.h;
    n.parent = parent;
    n.closed = false;
    nodes[node_count] = n;
    return node_count++;
}

/* Pick the open node with smallest f (ties: smaller h) */
static int pick_best(void) {
    int best = -1;
    int best_f = 999999;
    int best_h = 999999;
    for (int i = 0; i < node_count; i++) {
        if (nodes[i].closed) continue;
        if (nodes[i].f < best_f || (nodes[i].f == best_f && nodes[i].h < best_h)) {
            best_f = nodes[i].f;
            best_h = nodes[i].h;
            best = i;
        }
    }
    return best;
}

int main(void) {
    int sr = 0, sc = 0; /* start */
    int gr = 5, gc = 4; /* goal  */

    printf("=== A* Pathfinding: 6x5 Grid ===\n");
    printf("Start: (%d,%d)  Goal: (%d,%d)\n", sr, sc, gr, gc);
    printf("Obstacles: (2,2) (3,2) (1,3) (3,3)\n\n");

    /* Initialize start node */
    node_count = 0;
    find_or_create(sr, sc, 0, -1, gr, gc);

    int goal_idx = -1;

    /* ─── A* main loop ─── */
    while (1) {
        int cur = pick_best();
        if (cur < 0) break; /* no path */

        nodes[cur].closed = true;
        int r = nodes[cur].r;
        int c = nodes[cur].c;

        printf("Expand: (%d,%d) g=%d h=%d f=%d\n", r, c, nodes[cur].g, nodes[cur].h, nodes[cur].f);

        if (r == gr && c == gc) {
            goal_idx = cur;
            break;
        }

        /* 4-directional neighbors: up, down, left, right */
        int dr[] = {-1, 1, 0, 0};
        int dc[] = {0, 0, -1, 1};
        for (int d = 0; d < 4; d++) {
            int nr = r + dr[d];
            int nc = c + dc[d];
            if (nr < 0 || nr >= ROWS || nc < 0 || nc >= COLS) continue;
            if (OBS[nr][nc]) continue;
            int ng = nodes[cur].g + 1;
            find_or_create(nr, nc, ng, cur, gr, gc);
        }
    }

    /* ─── Reconstruct path ─── */
    if (goal_idx < 0) {
        printf("\nNo path found.\n");
        return 0;
    }

    int path[MAX_NODES];
    int path_len = 0;
    int idx = goal_idx;
    while (idx != -1) {
        path[path_len++] = idx;
        idx = nodes[idx].parent;
    }

    printf("\nPath found (length=%d):\n", path_len - 1);
    for (int i = path_len - 1; i >= 0; i--) {
        printf("  (%d,%d)", nodes[path[i]].r, nodes[path[i]].c);
        if (i > 0) printf(" ->");
    }
    printf("\n");

    return 0;
}
