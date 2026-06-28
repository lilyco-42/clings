/* 64_lockfree-ringbuffer.c — Lock-Free Ring Buffer (SPSC)
 *
 * 实现无锁 SPSC（单生产者单消费者）环形缓冲区，使用 C11 _Atomic 操作。
 *
 * 规格：
 *   - 容量：8 (BUFFER_SIZE)
 *   - 生产者写入 10 个 int (0-9)
 *   - 消费者读取 10 个 int
 *   - 两个原子索引：write_idx（生产者推进）、read_idx（消费者推进）
 *   - 消费者忙等待（自旋）直到数据可用
 *
 * 知识点：_Atomic、memory_order、SPSC 设计、
 *         缓存行伪共享、无锁编程。
 *
 * 验证：make test → 编译运行，管道 diff 比对 expected_output.txt
 */
#include <stdatomic.h>
#include <stdio.h>

#define BUFFER_SIZE 8
#define TOTAL_ITEMS 10

/* ─── Ring buffer data structure (provided) ─── */
static _Atomic int buffer[BUFFER_SIZE];
static _Atomic int write_idx = 0; /* producer advances this */
static _Atomic int read_idx = 0;  /* consumer advances this */

/* ─── TODO 1: 初始化环形缓冲区 ─── */
#error TODO: Implement init_buffer() — initialize buffer slots to -1, indices to 0.
/*
 * static void init_buffer(void)
 * 将所有 BUFFER_SIZE 个槽位初始化为 -1（哨兵值，表示"空"）。
 * 将 write_idx 和 read_idx 重置为 0。
 */

/* ─── TODO 2: 打印缓冲区状态 ─── */
#error TODO: Implement print_buffer(label) — print buffer contents with label.
/*
 * static void print_buffer(const char *label)
 * 格式："[label] buf=[...]"
 * 值 >= 0 打印数字，否则打印 "."。元素间用一个空格分隔。
 */

/* ─── TODO 3: 生产者写入 ─── */
#error TODO: Implement producer_write(value) — write one item into ring buffer.
/*
 * static void producer_write(int value)
 * 忙等待直到缓冲区非满（write_idx - read_idx < BUFFER_SIZE），
 * 写入 value 到 buffer[write_idx % BUFFER_SIZE]，
 * 打印操作信息，然后发布递增的 write_idx。
 */

/* ─── TODO 4: 消费者读取 ─── */
#error TODO: Implement consumer_read() — read one item from ring buffer.
/*
 * static int consumer_read(void)
 * 忙等待直到缓冲区非空（read_idx < write_idx），
 * 从 buffer[read_idx % BUFFER_SIZE] 读取值，标记槽位为 -1，
 * 打印操作信息，发布递增的 read_idx，返回读取的值。
 */

/* ─── TODO 5: 主函数 — 交错时间线 ─── */
#error TODO: Implement main() — run the interleaved producer/consumer timeline.
/*
 * int main(void)
 * 按交错时间线执行：
 *   Phase 1: 生产 5 个 (0..4)
 *   Phase 2: 消费 3 个 (0..2)
 *   Phase 3: 生产 5 个 (5..9) — 8 和 9 回绕到槽位 0 和 1
 *   Phase 4: 消费 7 个 (3..9)
 * 打印最终结果：Produced/Consumed 序列、最终索引、缓冲区是否为空。
 */
