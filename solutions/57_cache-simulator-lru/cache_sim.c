/* 57_cache-simulator-lru.c — 缓存模拟器 LRU 替换 (solution) */

#include <stdbool.h>
#include <stdio.h>

#define NUM_SETS 4
#define WAYS 2
#define BLOCK_SIZE 16

/* ─── Cache line structure ─── */
typedef struct {
    bool valid; /* valid bit */
    int tag;    /* tag bits */
    int lru_ts; /* last access timestamp (for LRU) */
} CacheLine;

static CacheLine cache[NUM_SETS][WAYS];
static int timestamp = 0; /* global timestamp counter */

/* ─── Address decomposition ─── */
static int get_set(int addr) { return (addr / BLOCK_SIZE) % NUM_SETS; }

static int get_tag(int addr) { return (addr / BLOCK_SIZE) / NUM_SETS; }

/* ─── Initialize cache ─── */
static void init_cache(void) {
    for (int s = 0; s < NUM_SETS; s++) {
        for (int w = 0; w < WAYS; w++) {
            cache[s][w].valid = false;
            cache[s][w].tag = 0;
            cache[s][w].lru_ts = 0;
        }
    }
    timestamp = 0;
}

/* ─── Access cache ─── */
static const char *access(int addr) {
    int set = get_set(addr);
    int tag = get_tag(addr);
    timestamp++;

    /* Check for hit */
    for (int w = 0; w < WAYS; w++) {
        if (cache[set][w].valid && cache[set][w].tag == tag) {
            cache[set][w].lru_ts = timestamp;
            return "hit";
        }
    }

    /* Miss: find empty way or LRU victim */
    int victim = -1;
    int min_ts = timestamp + 1;

    for (int w = 0; w < WAYS; w++) {
        if (!cache[set][w].valid) {
            victim = w;
            break;
        }
        if (cache[set][w].lru_ts < min_ts) {
            min_ts = cache[set][w].lru_ts;
            victim = w;
        }
    }

    /* Determine if it's an eviction */
    const char *result = cache[set][victim].valid ? "evict" : "miss";

    /* Fill the line */
    cache[set][victim].valid = true;
    cache[set][victim].tag = tag;
    cache[set][victim].lru_ts = timestamp;

    return result;
}

int main(void) {
    /* Fixed address sequence (12 accesses) */
    int addrs[] = {
        0,   16, 32, 48, /* 4 different sets, all miss */
        0,   64, 16, 80, /* 0=hit, 64=miss, 16=hit, 80=miss */
        128, 0,  32, 144 /* 128=miss, 0=hit, 32=conflict, 144=miss */
    };
    int n = sizeof(addrs) / sizeof(addrs[0]);

    init_cache();

    printf("=== Cache Simulator (2-way, 4 sets, 16B block) ===\n");
    printf("Address: [tag:2b|set:2b|offset:4b]\n\n");

    int hits = 0;

    for (int i = 0; i < n; i++) {
        int addr = addrs[i];
        int set = get_set(addr);
        int tag = get_tag(addr);
        const char *result = access(addr);

        printf("Access #%2d: addr=%3d  set=%d  tag=%d  %s\n", i + 1, addr, set, tag, result);

        if (result[0] == 'h') hits++; /* "hit" */
    }

    printf("\n--- Final Stats ---\n");
    printf("Total accesses: %d\n", n);
    printf("Hits: %d\n", hits);
    printf("Misses: %d\n", n - hits);
    printf("Hit rate: %.1f%%\n", (100.0 * hits) / n);

    return 0;
}
