/* 57_cache-simulator-lru.c — 缓存模拟器 LRU 替换
 *
 * 任务：1. 实现 init_cache()      — 初始化缓存所有行为无效
 *       2. 实现 access()          — 一次缓存访问 (hit/miss/evict)
 *       3. 实现 find_lru()        — 在组内找 LRU 行
 *       4. 补全 main() 中的主循环  — 遍历地址序列，统计命中
 *       5. 实现命中率计算          — 输出统计结果
 *
 * 背景：CPU 缓存是计算机体系结构的核心概念。本模拟器实现 2 路组相联
 *       (2-way set-associative) 缓存，使用 LRU(最近最少使用) 替换策略。
 *
 * 缓存参数：2 路 × 4 组 = 8 行，块大小 16B, 地址 8 位 (0-255)
 * 地址划分：[tag:2b|set:2b|offset:4b]
 *
 * 知识点：缓存映射/命中/缺失/冲突缺失/替换策略/局部性原理
 *
 * 验证：make test → 编译运行，管道 | diff 比对 expected_output.txt
 */
#include <stdbool.h>
#include <stdio.h>

#define NUM_SETS 4
#define WAYS 2
#define BLOCK_SIZE 16

/* ─── Cache line structure ─── */
typedef struct {
    bool valid; /* valid bit: 该行是否包含有效数据 */
    int tag;    /* tag bits: 标识数据块的高位地址 */
    int lru_ts; /* last access timestamp: 最后一次访问的时间戳 */
} CacheLine;

static CacheLine cache[NUM_SETS][WAYS];
static int timestamp = 0; /* global timestamp counter, 每次访问 +1 */

/* ─── Address decomposition (已提供，无需修改) ─── */

/* get_set: 从地址提取组索引 (set index)
 *   addr / BLOCK_SIZE 得到块号 (block number)
 *   块号 % NUM_SETS 得到组号
 *   8 位地址划分：[tag:2b|set:2b|offset:4b]
 *   例如 addr=64 → 块号=4 → set=0 */
static int get_set(int addr) { return (addr / BLOCK_SIZE) % NUM_SETS; }

/* get_tag: 从地址提取标记 (tag)
 *   块号 / NUM_SETS 得到标记
 *   例如 addr=64 → 块号=4 → tag=1 */
static int get_tag(int addr) { return (addr / BLOCK_SIZE) / NUM_SETS; }

/* ─── TODO 1: 初始化缓存 ─── */
#error TODO: Implement init_cache() — initialize all cache lines as invalid.
/*
 * static void init_cache(void) { ... }
 *
 * 遍历 NUM_SETS × WAYS:
 *   将每个 cache[s][w] 的 valid 设为 false
 *   将 tag 和 lru_ts 设为 0
 * 将全局 timestamp 重置为 0
 *
 * 提示：使用双重 for 循环，外层遍历组 s, 内层遍历路 w
 */

/* ─── TODO 2: 在组内找 LRU 行 ─── */
#error TODO: Implement find_lru(set) — return the way index of the LRU line.
/*
 * static int find_lru(int set) { ... }
 *
 * 在给定组 set 中，遍历 WAYS 路：
 *   找 lru_ts 最小的有效行 → 返回其 way 索引
 *   若存在无效行 (valid==false) → 优先返回无效行索引
 *
 * 提示：先检查无效行，再比较 lru_ts 找最小时间戳
 */

/* ─── TODO 3: 一次缓存访问 ─── */
#error TODO: Implement access(addr) — return "hit", "miss", or "evict".
/*
 * static const char *access(int addr) { ... }
 *
 * 步骤：
 *   1. set = get_set(addr), tag = get_tag(addr)
 *   2. timestamp++ (全局计数器递增)
 *   3. 查找命中：遍历组内 WAYS, 若 valid && tag 匹配：
 *        更新 lru_ts = timestamp; return "hit"
 *   4. 缺失处理：
 *        a) 调用 find_lru(set) 找 victim way
 *        b) 判断结果：若 victim 行 valid → "evict", 否则 → "miss"
 *        c) 填充 victim 行：valid=true, tag=tag, lru_ts=timestamp
 *        d) return 结果字符串
 *
 * 提示："evict" 表示替换了一个有效行 (冲突缺失)
 *       "miss"  表示填充了一个无效行 (冷缺失/容量缺失)
 */

int main(void) {
    /* 固定地址序列 (12 次访问，含命中/缺失/冲突缺失) */
    int addrs[] = {
        0,   16, 32, 48, /* 4 different sets, all miss */
        0,   64, 16, 80, /* 0=hit, 64=miss, 16=hit, 80=miss */
        128, 0,  32, 144 /* 128=miss, 0=conflict, 32=hit, 144=miss */
    };
    int n = sizeof(addrs) / sizeof(addrs[0]);

    /* ─── TODO 4: 调用 init_cache() ─── */
#error TODO: Call init_cache() before the main loop.
    /*
     * init_cache();
     */

    printf("=== Cache Simulator (2-way, 4 sets, 16B block) ===\n");
    printf("Address: [tag:2b|set:2b|offset:4b]\n\n");

    /* ─── TODO 5: 主循环 ─── */
#error TODO: Complete the main loop — iterate address sequence, count hits.
    /*
     * int hits = 0;
     * for (int i = 0; i < n; i++) {
     *     int addr = addrs[i];
     *     int set = get_set(addr);
     *     int tag = get_tag(addr);
     *     const char *result = access(addr);
     *
     *     printf("Access #%2d: addr=%3d  set=%d  tag=%d  %s\n",
     *            i + 1, addr, set, tag, result);
     *
     *     if (result[0] == 'h') hits++;
     * }
     */

    /* ─── TODO 6: 命中率计算 ─── */
#error TODO: Print final statistics — total, hits, misses, hit rate.
    /*
     * printf("\n--- Final Stats ---\n");
     * printf("Total accesses: %d\n", n);
     * printf("Hits: %d\n", hits);
     * printf("Misses: %d\n", n - hits);
     * printf("Hit rate: %.1f%%\n", (100.0 * hits) / n);
     */

    return 0;
}
