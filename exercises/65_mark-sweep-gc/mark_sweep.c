/* 65_mark-sweep-gc — Mark-Sweep Garbage Collector
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
 * 验证：make 构建后 clings run 65 / clings check 65（查看期望：clings tests 65）
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

/* ─── TODO 1: mark_recursive ─── */
#error TODO 1: Implement mark_recursive(int idx) — DFS mark from a given index.
/*
 * mark_recursive(int idx) should:
 *   1. Bounds check: if idx < 0 or idx >= HEAP_SIZE, return.
 *   2. If heap[idx].marked is already 1, return (prevents infinite loop on cycles).
 *   3. Set heap[idx].marked = 1.
 *   4. Loop through ref_count references: for each refs[i], call mark_recursive.
 *
 * This is a depth-first traversal of the object graph. The early-return on
 * already-marked objects is what makes it handle cycles correctly.
 *
 * Hint: the function is ~6 lines of code.
 */

/* ─── TODO 2: sweep ─── */
#error TODO 2: Implement sweep() — reclaim unmarked objects, return count.
/*
 * int sweep(void) should:
 *   1. Print "Sweeping (reclaiming unmarked objects):\n".
 *   2. Iterate through all HEAP_SIZE objects.
 *   3. For each object where marked == 0 (garbage):
 *      - Print "  OBJ%d reclaimed\n" with the index.
 *      - Reset refs[0] = refs[1] = -1, ref_count = 0.
 *      - Increment a local counter.
 *   4. For each object where marked == 1 (alive):
 *      - Reset marked = 0 to prepare for the NEXT GC cycle (standard sweep).
 *   5. Return the count of reclaimed objects.
 *
 * Note (标准 mark-sweep): sweep 会把存活对象的 mark 位清 0，这样下一轮 GC
 * 直接从"全未标记"开始，无需单独的清零遍历。因此 sweep 之后所有对象 marked
 * 都是 0，存活对象靠保留的 refs 与垃圾(refs 已清空)区分。
 * Hint: the function is ~14 lines of code.
 */

/* ─── TODO 3: print_objects ─── */
#error TODO 3: Implement print_objects(const char *label).
/*
 * void print_objects(const char *label) should:
 *   1. Print the label string with newline: printf("%s\n", label);
 *   2. Loop through all HEAP_SIZE objects.
 *   3. Print each object's state in the format:
 *      "  OBJ%-2d: marked=%d  refs=[%2d, %2d]  ref_count=%d\n"
 *      with fields: index, marked, refs[0], refs[1], ref_count.
 *
 * Hint: the function is ~5 lines of code.
 */

/* ─── TODO 4: gc_collect ─── */
#error TODO 4: Implement gc_collect() — orchestrate mark + sweep.
/*
 * void gc_collect(void) should:
 *   1. Print "=== Mark Phase ===\n".
 *   2. For each root in roots[]:
 *      - Print "Marking from root OBJ%d...\n" with roots[i].
 *      - Call mark_recursive(roots[i]).
 *   3. Print a blank line.
 *   4. Print "=== Sweep Phase ===\n".
 *   5. Call sweep() and store the returned count.
 *   6. Print a blank line.
 *   7. Print "=== GC Summary ===\n".
 *   8. Print "Objects collected: %d\n" with the sweep count.
 *   9. alive = HEAP_SIZE - collected  (sweep 已清零存活者的 mark 位，不能再用
 *      marked 计数；存活数 = 总数 - 回收数)。
 *   10. Print "Objects alive: %d\n" with the alive count.
 *
 * Hint: the function is ~20 lines of code.
 */

/* ─── Main ─── */
int main(void) {
/* ─── TODO 5: Complete main() ─── */
#error TODO 5: Initialize, print before-state, run GC, print after-state.
    /*
     * 1. Call init_heap().
     * 2. Print "=== Before GC: Initial Object Graph ===\n".
     * 3. Call print_objects("Object states (before marking):").
     * 4. Print a blank line (just printf("\n");).
     * 5. Call gc_collect().
     * 6. Print a blank line.
     * 7. Print "=== After GC: Final State ===\n".
     * 8. Call print_objects("Object states (after sweep):").
     * 9. Return 0.
     */
    return 0;
}
