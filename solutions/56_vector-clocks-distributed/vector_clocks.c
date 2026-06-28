/* 56_vector-clocks-distributed — solution
 *
 * Vector Clocks in a 3-node distributed system.
 * Fixed event sequence: P0 send→P1 recv→P1 local→P2 recv→P2 local→P0 recv
 * Demonstrates partial ordering and Happens-Before detection.
 */
#include <stdio.h>

#define N_NODES 3
#define N_EVENTS 6

typedef struct {
    int id;
    int clock[N_NODES];
} Node;

typedef struct {
    int type; /* 0=send, 1=recv, 2=local */
    int from; /* sender for recv */
    int to;   /* receiver for recv, or acting node */
    const char *desc;
} Event;

static Node nodes[N_NODES];

/* ---------- initialize all clocks to 0 ---------- */
static void init_clocks(void) {
    for (int i = 0; i < N_NODES; i++) {
        nodes[i].id = i;
        for (int j = 0; j < N_NODES; j++) nodes[i].clock[j] = 0;
    }
}

/* ---------- local event: increment own clock ---------- */
static void local_event(int node_id) { nodes[node_id].clock[node_id]++; }

/* ---------- send event: increment own clock, return snapshot ---------- */
static void send_event(int from, int to, int *msg_clock) {
    (void)to; /* kept for API symmetry */
    nodes[from].clock[from]++;
    for (int i = 0; i < N_NODES; i++) msg_clock[i] = nodes[from].clock[i];
}

/* ---------- recv event: merge + increment own clock ---------- */
static void recv_event(int to, const int *msg_clock) {
    /* merge: element-wise max */
    for (int i = 0; i < N_NODES; i++)
        if (msg_clock[i] > nodes[to].clock[i]) nodes[to].clock[i] = msg_clock[i];
    /* increment own */
    nodes[to].clock[to]++;
}

/* ---------- print all node clocks ---------- */
static void print_clocks(int event_num, const char *desc) {
    printf("E%d: %s\n", event_num, desc);
    for (int i = 0; i < N_NODES; i++) {
        printf("  P%d: [%d, %d, %d]\n", i, nodes[i].clock[0], nodes[i].clock[1], nodes[i].clock[2]);
    }
}

/* ---------- Happens-Before comparison ---------- */
static int happens_before(const int *a, const int *b) {
    /* a → b  iff  for all k: a[k] <= b[k]  AND  exists k: a[k] < b[k] */
    int lt = 0;
    for (int k = 0; k < N_NODES; k++) {
        if (a[k] > b[k]) return 0; /* not <= everywhere */
        if (a[k] < b[k]) lt = 1;
    }
    return lt;
}

static void test_hb(const char *label, const int *c1, const int *c2, int expected) {
    int result = happens_before(c1, c2);
    printf("  %s: %s (expected %s)\n", label, result ? "YES" : "NO", expected ? "YES" : "NO");
}

int main(void) {
    /* recorded clock snapshots after each event */
    int snapshots[N_EVENTS + 1][N_NODES][N_NODES];

    /* ── Event sequence definition ── */
    Event events[N_EVENTS] = {
        {0, 0, 1, "P0 SEND to P1"},   /* E1 */
        {1, 0, 1, "P1 RECV from P0"}, /* E2 */
        {2, -1, 1, "P1 LOCAL"},       /* E3 */
        {1, 1, 2, "P2 RECV from P1"}, /* E4 */
        {2, -1, 2, "P2 LOCAL"},       /* E5 */
        {1, 2, 0, "P0 RECV from P2"}, /* E6 */
    };

    init_clocks();

    printf("=== Vector Clocks: 3 Nodes (P0, P1, P2) ===\n\n");

    /* record initial snapshot */
    for (int i = 0; i < N_NODES; i++)
        for (int j = 0; j < N_NODES; j++) snapshots[0][i][j] = nodes[i].clock[j];

    for (int e = 0; e < N_EVENTS; e++) {
        Event *ev = &events[e];
        int msg_clock[N_NODES];

        switch (ev->type) {
            case 0: /* SEND */
                send_event(ev->from, ev->to, msg_clock);
                break;
            case 1: /* RECV */
                /* capture sender's current clock as the message */
                for (int i = 0; i < N_NODES; i++) msg_clock[i] = nodes[ev->from].clock[i];
                recv_event(ev->to, msg_clock);
                break;
            case 2: /* LOCAL */
                local_event(ev->to);
                break;
        }

        print_clocks(e + 1, ev->desc);

        /* save snapshot */
        for (int i = 0; i < N_NODES; i++)
            for (int j = 0; j < N_NODES; j++) snapshots[e + 1][i][j] = nodes[i].clock[j];
    }

    /* ── Happens-Before Tests ── */
    printf("\n=== Happens-Before Tests ===\n\n");
    printf("Rule: A→B iff for all k: A[k]<=B[k] AND exists k: A[k]<B[k]\n\n");

    /* Each test uses the snapshot of the acting node after its event */
    /* E1: P0 clock after event 1 */
    /* E2: P1 clock after event 2 */
    /* E3: P1 clock after event 3 */
    /* E4: P2 clock after event 4 */
    /* E5: P2 clock after event 5 */
    /* E6: P0 clock after event 6 */

    int *c1 = snapshots[1][0]; /* P0 after E1 */
    int *c2 = snapshots[2][1]; /* P1 after E2 */
    int *c3 = snapshots[3][1]; /* P1 after E3 */
    int *c4 = snapshots[4][2]; /* P2 after E4 */
    int *c5 = snapshots[5][2]; /* P2 after E5 */
    int *c6 = snapshots[6][0]; /* P0 after E6 */

    test_hb("E1→E2 (send→recv)", c1, c2, 1);
    test_hb("E2→E3 (same node)", c2, c3, 1);
    test_hb("E1→E3 (transitive)", c1, c3, 1);
    test_hb("E3→E4 (send→recv)", c3, c4, 1);
    test_hb("E3→E5 (transitive)", c3, c5, 1);
    test_hb("E1→E5 (chain)", c1, c5, 1);
    test_hb("E5→E1 (reverse)", c5, c1, 0);
    test_hb("E3→E6 (full chain)", c3, c6, 1);
    test_hb("E4→E6 (send→recv)", c4, c6, 1);

    return 0;
}
