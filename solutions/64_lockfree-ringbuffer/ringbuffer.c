/* 64_lockfree-ringbuffer.c — Lock-Free Ring Buffer (SPSC) (solution)
 *
 * A lock-free SPSC (Single Producer Single Consumer) ring buffer using
 * C11 _Atomic types. Demonstrates atomic operations, memory ordering,
 * busy-wait synchronization, and ring buffer wrapping.
 */
#include <stdatomic.h>
#include <stdio.h>

#define BUFFER_SIZE 8
#define TOTAL_ITEMS 10

/* ─── Ring buffer data structure ─── */
static _Atomic int buffer[BUFFER_SIZE];
static _Atomic int write_idx = 0; /* producer advances this */
static _Atomic int read_idx = 0;  /* consumer advances this */

/* ─── Initialize the ring buffer ─── */
static void init_buffer(void) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
        atomic_store_explicit(&buffer[i], -1, memory_order_relaxed);
    }
    atomic_store_explicit(&write_idx, 0, memory_order_relaxed);
    atomic_store_explicit(&read_idx, 0, memory_order_relaxed);
}

/* ─── Print current buffer state ─── */
static void print_buffer(const char *label) {
    printf("[%s] buf=[", label);
    for (int i = 0; i < BUFFER_SIZE; i++) {
        int val = atomic_load_explicit(&buffer[i], memory_order_relaxed);
        if (val >= 0)
            printf("%d", val);
        else
            printf(".");
        if (i < BUFFER_SIZE - 1) printf(" ");
    }
    printf("]");
}

/* ─── Producer: write one item into the ring buffer ─── */
static void producer_write(int value) {
    int w = atomic_load_explicit(&write_idx, memory_order_relaxed);
    int r = atomic_load_explicit(&read_idx, memory_order_acquire);

    /* Busy-wait: buffer full? (write_idx - read_idx == BUFFER_SIZE) */
    while (w - r >= BUFFER_SIZE) {
        r = atomic_load_explicit(&read_idx, memory_order_acquire);
    }

    /* Write the value into the ring buffer */
    atomic_store_explicit(&buffer[w % BUFFER_SIZE], value, memory_order_relaxed);

    printf("  [P] write %d at slot %d", value, w % BUFFER_SIZE);
    print_buffer(" after write");

    /* Publish: increment write_idx to signal consumer */
    atomic_store_explicit(&write_idx, w + 1, memory_order_release);
    printf("  w=%d r=%d\n", w + 1, r);
}

/* ─── Consumer: read one item from the ring buffer ─── */
static int consumer_read(void) {
    int r = atomic_load_explicit(&read_idx, memory_order_relaxed);
    int w = atomic_load_explicit(&write_idx, memory_order_acquire);

    /* Busy-wait: buffer empty? (read_idx == write_idx) */
    while (r >= w) {
        w = atomic_load_explicit(&write_idx, memory_order_acquire);
    }

    /* Read the value from the ring buffer */
    int value = atomic_load_explicit(&buffer[r % BUFFER_SIZE], memory_order_relaxed);
    /* Mark slot as consumed */
    atomic_store_explicit(&buffer[r % BUFFER_SIZE], -1, memory_order_relaxed);

    printf("  [C] read  %d at slot %d", value, r % BUFFER_SIZE);
    print_buffer(" after read ");

    /* Publish: increment read_idx to signal producer */
    atomic_store_explicit(&read_idx, r + 1, memory_order_release);
    printf("  w=%d r=%d\n", w, r + 1);

    return value;
}

int main(void) {
    printf("=== Lock-Free Ring Buffer (SPSC) ===\n");
    printf("Capacity: %d, Total items: %d\n\n", BUFFER_SIZE, TOTAL_ITEMS);

    init_buffer();

    printf("--- Interleaved Timeline ---\n");

    int consumed[TOTAL_ITEMS];
    int ci = 0; /* consume index */

    /* Interleaved execution: produce 5, consume 3, produce 5, consume 7.
     * This demonstrates:
     *  - Buffer filling up (first 5 produces)
     *  - Partial drain (3 consumes)
     *  - Wrapping: slots 0,1 reused (produce 8,9 wrap to slots 0,1)
     *  - Full drain (final 7 consumes) */

    /* Phase 1: produce 5 items (values 0..4) */
    for (int i = 0; i < 5; i++) producer_write(i);

    /* Phase 2: consume 3 items (values 0..2) */
    for (int i = 0; i < 3; i++) consumed[ci++] = consumer_read();

    /* Phase 3: produce 5 items (values 5..9) — wraps around! */
    for (int i = 5; i < 10; i++) producer_write(i);

    /* Phase 4: consume remaining 7 items (values 3..9) */
    for (int i = 0; i < 7; i++) consumed[ci++] = consumer_read();

    printf("\n--- Final Results ---\n");
    printf("Produced: [");
    for (int i = 0; i < TOTAL_ITEMS; i++) {
        printf("%d", i);
        if (i < TOTAL_ITEMS - 1) printf(", ");
    }
    printf("]\n");
    printf("Consumed: [");
    for (int i = 0; i < TOTAL_ITEMS; i++) {
        printf("%d", consumed[i]);
        if (i < TOTAL_ITEMS - 1) printf(", ");
    }
    printf("]\n");

    int final_w = atomic_load_explicit(&write_idx, memory_order_relaxed);
    int final_r = atomic_load_explicit(&read_idx, memory_order_relaxed);
    printf("Final write_idx: %d\n", final_w);
    printf("Final read_idx: %d\n", final_r);
    printf("Buffer empty: %s\n", (final_w == final_r) ? "yes" : "no");

    return 0;
}
