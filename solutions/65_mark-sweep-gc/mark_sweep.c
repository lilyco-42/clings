/* 65_mark-sweep-gc — Mark-Sweep Garbage Collector (solution)
 *
 * Task: Implement a simple mark-sweep GC on a fixed object graph.
 * The heap has 16 objects with hardcoded references.
 * Root set = {OBJ0, OBJ3, OBJ5}.
 *
 * Knowledge points:
 *   - GC fundamentals: reachability, root set, mark phase, sweep phase
 *   - DFS graph traversal for marking live objects
 *   - Tri-color abstraction (white/gray/black)
 *   - Memory fragmentation from non-moving collectors
 *
 * Verification:
 *   ./mark_sweep
 *   (output matches expected_output.txt)
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define HEAP_SIZE 16

/* ─── Object structure ─── */
typedef struct {
    int marked;    /* 0 = not marked, 1 = marked (reachable) */
    int refs[2];   /* up to 2 references (indices into heap[], -1 = none) */
    int ref_count; /* actual number of references (0, 1, or 2) */
} Object;

/* ─── Global heap ─── */
static Object heap[HEAP_SIZE];

/* ─── Root set indices ─── */
static int roots[] = {0, 3, 5};
static int root_count = 3;

/* ─── Initialize object graph ─── */
static void init_heap(void) {
    /* Zero everything first */
    memset(heap, 0, sizeof(heap));

    /* Set all refs to -1 (no reference) */
    for (int i = 0; i < HEAP_SIZE; i++) {
        heap[i].refs[0] = -1;
        heap[i].refs[1] = -1;
    }

    /*
     * Object graph (ASCII diagram):
     *
     *   ROOTS: [OBJ0]  [OBJ3]  [OBJ5]
     *             │       │       │
     *             ▼       ▼       ▼
     *   OBJ0 ──→ OBJ1   OBJ3 ──→ OBJ4   OBJ5 ──→ OBJ6
     *             │                         │
     *             ▼                         ▼
     *            OBJ2                      OBJ7
     *             ▲                         │
     *             └─────────────────────────┘ (cycle: OBJ7→OBJ5)
     *
     *   OBJ8: isolated (no incoming refs, not in root set)
     *   OBJ9-OBJ15: unused (all refs = -1)
     *
     *   Reachable from roots: OBJ0,OBJ1,OBJ2,OBJ3,OBJ4,OBJ5,OBJ6,OBJ7
     *   Unreachable (garbage): OBJ8, OBJ9-OBJ15
     */

    /* OBJ0 → OBJ1 */
    heap[0].refs[0] = 1;
    heap[0].ref_count = 1;

    /* OBJ1 → OBJ2 */
    heap[1].refs[0] = 2;
    heap[1].ref_count = 1;

    /* OBJ2: no outgoing refs (leaf) */
    heap[2].ref_count = 0;

    /* OBJ3 → OBJ4 */
    heap[3].refs[0] = 4;
    heap[3].ref_count = 1;

    /* OBJ4: leaf */
    heap[4].ref_count = 0;

    /* OBJ5 → OBJ6 */
    heap[5].refs[0] = 6;
    heap[5].ref_count = 1;

    /* OBJ6 → OBJ7 */
    heap[6].refs[0] = 7;
    heap[6].ref_count = 1;

    /* OBJ7 → OBJ5 (cycle back!) */
    heap[7].refs[0] = 5;
    heap[7].ref_count = 1;

    /* OBJ8: isolated object (no incoming refs) */
    heap[8].ref_count = 0;

    /* OBJ9-OBJ15: unused */
}

/* ─── Mark phase: DFS from root, mark all reachable objects ─── */
static void mark_recursive(int idx) {
    if (idx < 0 || idx >= HEAP_SIZE) return; /* bounds check */
    if (heap[idx].marked) return;            /* already marked (handles cycles) */

    heap[idx].marked = 1; /* mark as reachable */

    for (int i = 0; i < heap[idx].ref_count; i++) {
        mark_recursive(heap[idx].refs[i]); /* DFS into children */
    }
}

/* ─── Sweep phase: reclaim unmarked objects ─── */
static int sweep(void) {
    int collected = 0;

    printf("Sweeping (reclaiming unmarked objects):\n");
    for (int i = 0; i < HEAP_SIZE; i++) {
        if (!heap[i].marked) {
            printf("  OBJ%d reclaimed\n", i);
            /* Reset the object: clear refs */
            heap[i].refs[0] = -1;
            heap[i].refs[1] = -1;
            heap[i].ref_count = 0;
            collected++;
        }
    }
    return collected;
}

/* ─── Print current object states ─── */
static void print_objects(const char *label) {
    printf("%s\n", label);
    for (int i = 0; i < HEAP_SIZE; i++) {
        printf("  OBJ%-2d: marked=%d  refs=[%2d, %2d]  ref_count=%d\n", i, heap[i].marked, heap[i].refs[0],
               heap[i].refs[1], heap[i].ref_count);
    }
}

/* ─── GC collect: mark + sweep ─── */
static void gc_collect(void) {
    /* Mark phase: DFS from each root */
    printf("=== Mark Phase ===\n");
    for (int i = 0; i < root_count; i++) {
        printf("Marking from root OBJ%d...\n", roots[i]);
        mark_recursive(roots[i]);
    }
    printf("\n");

    /* Sweep phase: reclaim unmarked */
    printf("=== Sweep Phase ===\n");
    int collected = sweep();
    printf("\n");

    printf("=== GC Summary ===\n");
    printf("Objects collected: %d\n", collected);

    /* Count survivors */
    int alive = 0;
    for (int i = 0; i < HEAP_SIZE; i++) {
        if (heap[i].marked) alive++;
    }
    printf("Objects alive: %d\n", alive);
}

/* ─── Main ─── */
int main(void) {
    init_heap();

    printf("=== Before GC: Initial Object Graph ===\n");
    print_objects("Object states (before marking):");
    printf("\n");

    gc_collect();

    printf("\n");
    printf("=== After GC: Final State ===\n");
    print_objects("Object states (after sweep):");

    return 0;
}
