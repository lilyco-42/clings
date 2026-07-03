# 无锁环形缓冲区 — C11 _Atomic SPSC 实现

## 1. 课程任务

实现一个无锁 (lock-free) 的单生产者单消费者 (SPSC) 环形缓冲区，使用 C11 `_Atomic` 类型和原子操作。完成五个核心任务：

- **TODO 1**: 实现 `init_buffer()` — 初始化缓冲区槽位和原子索引
- **TODO 2**: 实现 `print_buffer()` — 可视化输出缓冲区当前状态
- **TODO 3**: 实现 `producer_write()` — 生产者写入一个元素，含忙等待满检测
- **TODO 4**: 实现 `consumer_read()` — 消费者读取一个元素，含忙等待空检测
- **TODO 5**: 实现 `main()` — 交错时间线：生产 5 个→消费 3 个→生产 5 个→消费 7 个

> **重要：本实现是「单线程确定性时间线」，并不真正并发。** `main()` 在**同一个线程**里顺序执行 Phase 1-4（生产 5 → 消费 3 → 生产 5 → 消费 7），用于演示 SPSC 原语的 API 与 `memory_order` 的**正确放置**。因为单线程内没有真正的跨核重排竞争：忙等待循环不会真的自旋（进入时条件已满足），`memory_order` 也**没有可观测的运行时效果**——好处是输出完全确定、判分稳定。**真正的并发**需要把生产者、消费者放到不同线程（`pthread`，编译时加 `-pthread`），那时 acquire/release 配对才会真正生效，且输出顺序会变得**非确定**。下文 §4.2、§5 中出现的"写入线程/读取线程""跨线程 happens-before""有锁 vs 无锁 pthread"等表述，讲的是这些原语在**真并发**下的语义，请对照本说明阅读。

验证方式：构建用 `make`；判分/自测用 `clings run`/`clings watch`（clings 捕获程序 stdout 后与内置测试用例逐行比对）；查看期望输出用 `clings tests 64`。

---

## 2. 前置知识

### 2.1 为什么要无锁？

传统多线程同步使用互斥锁 (mutex):

```c
pthread_mutex_lock(&mutex);
// 临界区：操作共享数据
pthread_mutex_unlock(&mutex);
```

锁的四个核心问题：

| 问题           | 描述                             | 影响                     |
| -------------- | -------------------------------- | ------------------------ |
| **阻塞等待**   | 持锁线程阻塞其他所有线程         | 上下文切换开销 (~1-10μs) |
| **死锁**       | 多锁获取顺序不当                 | 程序永久挂起             |
| **优先级反转** | 低优先级线程持锁阻塞高优先级线程 | 实时系统灾难             |
| **锁竞争**     | 高并发下吞吐量急剧下降           | 不可扩展                 |

**无锁编程** (lock-free programming) 使用原子 CPU 指令 (CAS, FAA, LL/SC)
直接在共享数据上操作，无需互斥锁。C11 通过 `<stdatomic.h>` 提供了标准化接口。

### 2.2 什么是 SPSC？

SPSC = **S**ingle **P**roducer **S**ingle **C**onsumer

```
┌──────────┐     write_idx     ┌──────────────┐     read_idx      ┌──────────┐
│ Producer │ ────────────────→ │  Ring Buffer │ ────────────────→ │ Consumer │
│ (1线程)  │                   │  (capacity=8) │                   │ (1线程)  │
└──────────┘                   └──────────────┘                   └──────────┘
```

SPSC 是最简单的无锁队列场景：

- 只有一个线程写入 (不需要处理多生产者竞争)
- 只有一个线程读取 (不需要处理多消费者竞争)
- 写入和读取操作完全解耦，各管各的索引

扩展到 MPMC (多生产者多消费者) 需要 CAS 循环，复杂度显著增加。

### 2.3 为什么用环形缓冲区？

```
线性队列 (每次出队移动所有元素):
  [A][B][C][D][_] → 出队A → [B][C][D][_][_]  (移动 O(n) 次)

环形缓冲区 (移动指针, 数据不动):
  [A][B][C][D]     write_idx=4, read_idx=0
  出队A → read_idx=1  (O(1), 数据仍在原位)
  [.][B][C][D]     (逻辑上A已出队)
```

优势：

1. **O(1)** 入队和出队操作
2. 固定内存分配，无动态内存管理
3. 缓存友好 — 连续内存访问

### 2.4 C11 原子操作基础

`_Atomic` 关键字声明原子类型：

```c
_Atomic int counter = 0;    // 原子整数
_Atomic int buffer[8];      // 原子整数数组
```

原子操作函数 (带显式内存顺序):

```c
// 原子存储
atomic_store_explicit(&var, value, memory_order_xxx);

// 原子加载
int v = atomic_load_explicit(&var, memory_order_xxx);
```

**关键**: `_Atomic` 保证对该变量的读写是原子的 (不可分割的),
不会出现"读了一半被另一个线程写入打断"的情况。

---

## 3. 环形缓冲区结构详解

### 3.1 数据结构 ASCII 图

```
                 BUFFER_SIZE = 8
    ┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
    │ [0] │ [1] │ [2] │ [3] │ [4] │ [5] │ [6] │ [7] │
    └─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
      ↑                                   ↑
   read_idx % 8                      write_idx % 8
   (下一个要读的槽位)                (下一个要写的槽位)

索引语义:
  write_idx = 生产者已写入的元素总数 (单调递增, 永不减少)
  read_idx  = 消费者已读取的元素总数 (单调递增, 永不减少)

实际槽位:
  写入位置 = write_idx % BUFFER_SIZE
  读取位置 = read_idx  % BUFFER_SIZE

缓冲区状态判断:
  可读元素数 = write_idx - read_idx
  可写槽位数 = BUFFER_SIZE - (write_idx - read_idx)

  空: write_idx == read_idx               (可读=0)
  满: write_idx - read_idx == BUFFER_SIZE  (可写=0)
```

### 3.2 为什么索引不取模？

```
错误做法 (取模索引):
  write_idx = (write_idx + 1) % BUFFER_SIZE;
  → write_idx 始终在 [0, BUFFER_SIZE-1] 范围内
  → 无法区分"缓冲区满"和"缓冲区空"!

  write_idx=0, read_idx=0  → 空? 还是 满(刚好绕了一圈)?

正确做法 (单调递增索引):
  write_idx 和 read_idx 永远递增
  → write_idx - read_idx = 实际元素数量
  → 在访问 buffer[] 时才用 % BUFFER_SIZE 计算槽位
  → 可区分空(差=0)和满(差=BUFFER_SIZE)
```

### 3.3 回绕 (Wrapping) 示意

```
容量 8, 写入 10 个元素:

write_idx=0:  写槽0    buf=[0 . . . . . . .]
write_idx=1:  写槽1    buf=[0 1 . . . . . .]
write_idx=2:  写槽2    buf=[0 1 2 . . . . .]
write_idx=3:  写槽3    buf=[0 1 2 3 . . . .]
write_idx=4:  写槽4    buf=[0 1 2 3 4 . . .]
write_idx=5:  写槽5    buf=[0 1 2 3 4 5 . .]
write_idx=6:  写槽6    buf=[0 1 2 3 4 5 6 .]
write_idx=7:  写槽7    buf=[0 1 2 3 4 5 6 7]  ← 满了!

-- 消费者读了3个, read_idx 从 0 → 3 --

write_idx=8:  写槽0    buf=[8 1 2 . 4 5 6 7]  ← 回绕到槽0!
write_idx=9:  写槽1    buf=[8 9 2 . 4 5 6 7]  ← 回绕到槽1!

关键: write_idx=8 时, 8 % 8 = 0, 覆盖了槽0 (已被消费的旧数据)
```

---

## 4. 内存顺序详解

### 4.1 C11 内存顺序分类

C11 定义了 6 种内存顺序 (memory order), 从弱到强：

```
弱 ──────────────────────────────────────────────→ 强
relaxed  consume  acquire  release  acq_rel  seq_cst
  ↑                  ↑        ↑                   ↑
  无顺序约束         读屏障    写屏障             全屏障
```

本题使用三种：

| memory_order | 约束                     | 典型用途     | 本题使用场景          |
| ------------ | ------------------------ | ------------ | --------------------- |
| `relaxed`    | 只保证原子性，无顺序保证 | 独立计数器   | buffer 读写、打印     |
| `acquire`    | 后续操作不重排到此之前   | 读取共享标志 | consumer 读 write_idx |
| `release`    | 之前操作不重排到此之后   | 发布数据     | producer 写 write_idx |

### 4.2 acquire-release 配对原理

```
Producer (写入线程):
  ┌─────────────────────────────────────────┐
  │ 1. 写入数据到 buffer[slot]               │  ← relaxed
  │    (普通写入, 可能还在 store buffer 中)   │
  │                                          │
  │ 2. atomic_store(&write_idx, w+1, release)│  ← release 屏障
  │    ★ 保证: 步骤1 的所有写入对后续可见     │
  └─────────────────────────────────────────┘
                    │
                    │  happens-before
                    ▼
  ┌─────────────────────────────────────────┐
  │ 3. atomic_load(&write_idx, acquire)      │  ← acquire 屏障
  │    ★ 保证: 看到 write_idx 更新后,         │
  │            步骤1 的写入也一定可见          │
  │                                          │
  │ 4. 读取 buffer[slot] 的数据              │  ← relaxed
  │    (一定能读到完整的数据!)                │
  └─────────────────────────────────────────┘
Consumer (读取线程):
```

**如果不用 release/acquire 而全部用 relaxed**:

- CPU 可能将步骤 2 重排到步骤 1 之前 (store-store 重排)
- 消费者可能看到更新的 write_idx 但读到旧的 buffer 数据
- 这在 x86 上不太可能发生 (x86 有较强的内存模型),
  但在 ARM/PowerPC 等弱内存模型架构上会发生！

### 4.3 内存顺序使用速查

```
场景: "我写入数据, 然后告诉别人数据准备好了"
  → 写数据用 relaxed, 写标志用 release

场景: "我看到标志, 然后读取对应的数据"
  → 读标志用 acquire, 读数据用 relaxed

场景: "只是一个计数器, 不保护其他数据"
  → 用 relaxed 即可

场景: "不确定该用什么"
  → 用 seq_cst (最安全但最慢, 适合调试阶段)
```

---

## 5. 原子操作对比表

### 5.1 有锁 vs 无锁

| 维度            | 互斥锁 (mutex)   | 无锁 _Atomic (SPSC)    |
| --------------- | ---------------- | ---------------------- |
| 阻塞            | 是，线程进入睡眠 | 否，忙等待 (spin-wait) |
| 上下文切换      | 有 (~1-10μs)     | 无                     |
| 临界区长度      | 适合长临界区     | 适合极短操作           |
| 死锁风险        | 有               | 无 (无锁)              |
| 优先级反转      | 可能             | 无                     |
| CPU 使用        | 等待时释放 CPU   | 忙等待消耗 CPU         |
| 实现复杂度      | 低 (pthread API) | 中 (需理解内存顺序)    |
| 适用场景        | 通用多线程       | 高频低延迟队列         |
| 吞吐量 (低竞争) | 好               | 极好                   |
| 吞吐量 (高竞争) | 差               | SPSC 不受影响          |

### 5.2 忙等待 vs 阻塞等待

```
忙等待 (spin-wait / busy-wait):
  while (condition_not_met) {
      // 反复检查, 消耗 CPU 周期
  }

  优点: 唤醒延迟 ~0 (条件满足立即继续)
  缺点: 浪费 CPU (适合等待时间极短的场景)
  本题使用: 生产者等待缓冲区非满, 消费者等待缓冲区非空

阻塞等待 (blocking wait):
  pthread_cond_wait(&cond, &mutex);
  // 线程进入睡眠, 被唤醒后继续

  优点: 不浪费 CPU
  缺点: 唤醒延迟 ~1-10μs (上下文切换)
  适合: 等待时间可能较长的场景
```

### 5.3 内存顺序组合效果

| 生产者写 | 消费者读 | 效果                     |
| -------- | -------- | ------------------------ |
| relaxed  | relaxed  | ❌ 数据可能不可见        |
| release  | acquire  | ✅ 正确的 happens-before |
| release  | relaxed  | ⚠️ 部分保证 (消费者侧弱) |
| relaxed  | acquire  | ⚠️ 部分保证 (生产者侧弱) |
| seq_cst  | seq_cst  | ✅ 最强保证 (但最慢)     |

---

## 6. 完整交错时间线

### 6.1 逐步状态追踪

**初始状态**: `write_idx=0, read_idx=0`

```
buf=[. . . . . . . .]  (空缓冲区)
```

**Phase 1 — 生产 5 个 (值 0..4)**:

```
Step 1: P 写 0 @ 槽 0    buf=[0 . . . . . . .]  w=1 r=0
Step 2: P 写 1 @ 槽 1    buf=[0 1 . . . . . .]  w=2 r=0
Step 3: P 写 2 @ 槽 2    buf=[0 1 2 . . . . .]  w=3 r=0
Step 4: P 写 3 @ 槽 3    buf=[0 1 2 3 . . . .]  w=4 r=0
Step 5: P 写 4 @ 槽 4    buf=[0 1 2 3 4 . . .]  w=5 r=0
```

此时可读元素：w-r = 5, 可写槽位：8-5 = 3

**Phase 2 — 消费 3 个 (值 0..2)**:

```
Step 6:  C 读 0 @ 槽 0    buf=[. 1 2 3 4 . . .]  w=5 r=1
Step 7:  C 读 1 @ 槽 1    buf=[. . 2 3 4 . . .]  w=5 r=2
Step 8:  C 读 2 @ 槽 2    buf=[. . . 3 4 . . .]  w=5 r=3
```

此时可读元素：5-3 = 2, 可写槽位：8-2 = 6

**Phase 3 — 生产 5 个 (值 5..9)**:

```
Step 9:  P 写 5 @ 槽 5    buf=[. . . 3 4 5 . .]  w=6 r=3
Step 10: P 写 6 @ 槽 6    buf=[. . . 3 4 5 6 .]  w=7 r=3
Step 11: P 写 7 @ 槽 7    buf=[. . . 3 4 5 6 7]  w=8 r=3
Step 12: P 写 8 @ 槽 0    buf=[8 . . 3 4 5 6 7]  w=9 r=3   ← 回绕!
Step 13: P 写 9 @ 槽 1    buf=[8 9 . 3 4 5 6 7]  w=10 r=3  ← 回绕!
```

此时可读元素：10-3 = 7, 可写槽位：8-7 = 1
注意：槽 0 和槽 1 被新数据 8 和 9 覆盖 (旧数据 0 和 1 已被消费)

**Phase 4 — 消费 7 个 (值 3..9)**:

```
Step 14: C 读 3 @ 槽 3    buf=[8 9 . . 4 5 6 7]  w=10 r=4
Step 15: C 读 4 @ 槽 4    buf=[8 9 . . . 5 6 7]  w=10 r=5
Step 16: C 读 5 @ 槽 5    buf=[8 9 . . . . 6 7]  w=10 r=6
Step 17: C 读 6 @ 槽 6    buf=[8 9 . . . . . 7]  w=10 r=7
Step 18: C 读 7 @ 槽 7    buf=[8 9 . . . . . .]  w=10 r=8
Step 19: C 读 8 @ 槽 0    buf=[. 9 . . . . . .]  w=10 r=9
Step 20: C 读 9 @ 槽 1    buf=[. . . . . . . .]  w=10 r=10
```

**最终状态**: `write_idx=10, read_idx=10, 缓冲区空`

### 6.2 时间线总结表

| Step | 操作 | 值  | 槽位 | w   | r   | 缓冲区状态   | 备注       |
| ---- | ---- | --- | ---- | --- | --- | ------------ | ---------- |
| 1    | P 写 | 0   | 0    | 1   | 0   | `[0.......]` |            |
| 2    | P 写 | 1   | 1    | 2   | 0   | `[01......]` |            |
| 3    | P 写 | 2   | 2    | 3   | 0   | `[012.....]` |            |
| 4    | P 写 | 3   | 3    | 4   | 0   | `[0123....]` |            |
| 5    | P 写 | 4   | 4    | 5   | 0   | `[01234...]` |            |
| 6    | C 读 | 0   | 0    | 5   | 1   | `[.1234...]` |            |
| 7    | C 读 | 1   | 1    | 5   | 2   | `[..234...]` |            |
| 8    | C 读 | 2   | 2    | 5   | 3   | `[...34...]` |            |
| 9    | P 写 | 5   | 5    | 6   | 3   | `[...345..]` |            |
| 10   | P 写 | 6   | 6    | 7   | 3   | `[...3456.]` |            |
| 11   | P 写 | 7   | 7    | 8   | 3   | `[...34567]` |            |
| 12   | P 写 | 8   | 0    | 9   | 3   | `[8..34567]` | **回绕！** |
| 13   | P 写 | 9   | 1    | 10  | 3   | `[89.34567]` | **回绕！** |
| 14   | C 读 | 3   | 3    | 10  | 4   | `[89..4567]` |            |
| 15   | C 读 | 4   | 4    | 10  | 5   | `[89...567]` |            |
| 16   | C 读 | 5   | 5    | 10  | 6   | `[89....67]` |            |
| 17   | C 读 | 6   | 6    | 10  | 7   | `[89.....7]` |            |
| 18   | C 读 | 7   | 7    | 10  | 8   | `[89......]` |            |
| 19   | C 读 | 8   | 0    | 10  | 9   | `[.9......]` |            |
| 20   | C 读 | 9   | 1    | 10  | 10  | `[........]` | 空         |

---

## 7. 边界情况分析

| 场景             | 生产者行为        | 消费者行为           | 结果                   |
| ---------------- | ----------------- | -------------------- | ---------------------- |
| 缓冲区空         | —                 | 忙等待 (r >= w)      | 消费者 spin 直到有数据 |
| 缓冲区满         | 忙等待 (w-r >= 8) | —                    | 生产者 spin 直到有空间 |
| 缓冲区半满       | 正常写入          | 正常读取             | 两者都不等待           |
| 回绕写入         | w%8 回到 0,1...   | 正常                 | 覆盖已消费的旧数据     |
| 生产者比消费者快 | 写满 8 个后 spin  | 逐步消费腾空间       | 生产者在第 9 个时阻塞  |
| 消费者比生产者快 | 正常              | 读完后 spin 等新数据 | 消费者在缓冲区空时阻塞 |

---

## 8. 常见错误

| 错误                           | 后果                             | 正确做法                                       |
| ------------------------------ | -------------------------------- | ---------------------------------------------- |
| 索引取模存储                   | 无法区分空和满                   | 索引单调递增，只在访问 buffer 时 % BUFFER_SIZE |
| 忘记 `#include <stdatomic.h>`  | 编译错误：`_Atomic` 未定义       | 必须包含此头文件                               |
| 全部用 `memory_order_relaxed`  | 数据竞争：消费者可能读到半写数据 | 生产者 release, 消费者 acquire                 |
| 满判断用 `w - r > BUFFER_SIZE` | 多等一轮 (应为 `>=`)             | `while (w - r >= BUFFER_SIZE)`                 |
| 空判断用 `r == w` 只检查一次   | 忙等待可能永远不退出             | 在 while 循环内重新加载 w                      |
| 消费后不标记槽位为 -1          | print_buffer 输出旧数据          | `atomic_store(&buf[slot], -1, relaxed)`        |
| 忘记更新索引 (release)         | 对方线程永远看不到新数据         | `atomic_store(&write_idx, w+1, release)`       |
| 用 `=` 直接赋值原子变量        | 编译器可能拒绝或丢失原子性       | 使用 `atomic_store/load_explicit`              |
| 主循环中先消费再生产           | 缓冲区空，消费者永远等待         | 交错顺序：先生产填充，再消费腾空间             |
| 打印时用 `%d` 输出 -1          | 输出 `-1` 而非 `.`               | 检查 `val >= 0` 再决定打印什么                 |

---

## 9. 重要知识点

### 9.1 为什么 SPSC 不需要 CAS？

CAS (Compare-And-Swap) 是多生产者/多消费者场景需要的：

```c
// MPMC 需要 CAS:
do {
    old = atomic_load(&write_idx);
    new = old + 1;
} while (!atomic_compare_exchange_weak(&write_idx, &old, new));
//  ← 多个生产者竞争同一个 write_idx, 需要 CAS 重试循环
```

SPSC 只有一个生产者更新 write_idx, 没有竞争，所以只需简单的 load/store。

### 9.2 内存屏障的真实代价

```
x86 (强内存模型):
  - acquire 读 ≈ 普通读 (编译器屏障即可)
  - release 写 ≈ 普通写 (编译器屏障即可)
  - 只有 StoreLoad 屏障 (mfence) 才需要真正 CPU 指令
  → 本题在 x86 上用 relaxed 可能也"正常工作" (不要依赖!)

ARM/PowerPC (弱内存模型):
  - acquire 读需要 dmb/ldar 指令
  - release 写需要 dmb/stlr 指令
  - 用错内存顺序 → 数据竞争, 难以复现的 bug
  → 本题的正确内存顺序在这些架构上至关重要!
```

### 9.3 缓存行与伪共享

```
CPU0 缓存行 (64B):
┌─────────────────────────────────────────────────────┐
│ buffer[0..7] (32B) │ write_idx (4B) │ read_idx (4B) │ ...
└─────────────────────────────────────────────────────┘

如果 write_idx 和 read_idx 在同一个缓存行:
  生产者更新 write_idx → CPU0 的缓存行被标记为 modified
  → CPU1 (运行消费者) 的对应缓存行失效
  → 消费者下次读 read_idx 需要从 CPU0 重新加载 (cache miss)
  → 即使消费者从不访问 write_idx!

解决方案: 添加 padding
  _Atomic int write_idx = 0;
  char _pad1[60];  // 填充到 64 字节对齐
  _Atomic int read_idx = 0;
  char _pad2[60];
```

本题 SPSC 场景伪共享影响较小 (只有 2 个线程，且访问模式互补),
但在高性能场景下可降低 20-50% 的吞吐量。

### 9.4 无锁 ≠ 无等待

```
lock-free:  至少有一个线程能在有限步骤内完成 (整体进度保证)
wait-free:  每个线程都能在有限步骤内完成 (单线程进度保证)

SPSC 环形缓冲区:
  生产者写入 → O(1), 忙等待时间取决于消费者速度
  消费者读取 → O(1), 忙等待时间取决于生产者速度
  → lock-free (整体在推进), 但非 wait-free (单个线程可能等待)
```

### 9.5 为什么用 int 而不是 uint?

本题使用 `int` 便于用 -1 作为哨兵值标记空槽位。
在生产代码中，通常使用 `size_t` 或 `uint32_t` 作为索引类型，
并用额外的标志位区分空/满状态。

另外注意：`write_idx`/`read_idx` 是**单调递增**的 `_Atomic int`，长时间运行会溢出——而**有符号整数溢出是未定义行为 (UB)**。本题只递增到 10，远不会触发；生产代码通常改用无符号类型（其回绕行为是有良定义的），并在容量为 2 的幂时用位掩码代替取模。

---

## 10. 课堂讨论

### 10.1 如果把 BUFFER_SIZE 改为 4, 程序行为如何变化？

生产者会在写入第 5 个元素时阻塞 (4 个槽位全满)。
但由于我们的交错时间线是先生产 5 个再消费，第 5 个生产会永远等待
(消费者还没开始读), 程序死锁。

**教训**: SPSC 的时序必须正确，生产者不能比消费者快太多。

### 10.2 如果全部用 memory_order_seq_cst 会怎样？

程序正确但更慢。`seq_cst` 是最强的内存顺序，在 x86 上可能需要 `mfence`
指令 (在不需要的场景下浪费 ~100 CPU 周期)。本题中 `release/acquire`
是最优选择。

### 10.3 为什么本题是单线程但使用原子操作？

本题是教学演示：在单线程中交错执行生产者和消费者操作，
模拟多线程的行为。原子操作确保即使在同一线程中，
操作也遵循正确的内存顺序约束。

在真实场景中，生产者和消费者分别在不同线程/核心上运行，
`_Atomic` 和正确的 `memory_order` 保证它们之间正确的数据传递。

### 10.4 消费后为什么要把槽位设为 -1？

两个原因：

1. `print_buffer()` 使用 -1 来判断槽位是否为空，设为 -1 后输出 "."
2. 在真实多线程场景中，消费者清空槽位是良好的编程实践，
   防止后续调试时看到"幽灵数据"

### 10.5 如何在真实项目中使用无锁队列？

推荐使用成熟库而非自己实现：

- **C**: `concurrencykit` (ck_ring), `liburcu`
- **C++**: `boost::lockfree::spsc_queue`, `folly::ProducerConsumerQueue`
- **Rust**: `crossbeam::channel`, `tokio::sync::mpsc`

自实现的风险：内存顺序错误在 x86 上可能"碰巧正常",
但在 ARM 服务器上崩溃 — 这类 bug 极难调试。

---

## 11. 后续衔接

- **前置课程**: Lesson 49 `dining-philosophers-sync` (并发概念与死锁)、Lesson 35 `queue_base` (队列数据结构基础)
- **后续课程**: 操作系统中的无锁数据结构 (Linux 内核 kfifo)、RCU (Read-Copy-Update)、Hazard Pointers
- **实际应用**:
  - Linux 内核 `kfifo` (内核环形缓冲区)
  - DPDK 无锁环形缓冲区 (高性能网络包处理)
  - LMAX Disruptor (高性能线程间消息传递)
  - 音频/视频流缓冲区
  - 日志系统的异步缓冲

---

## 12. 参考资料

1. ISO/IEC 9899:2011, Section 7.17 — `<stdatomic.h>` specification
2. Preshing, J. _An Introduction to Lock-Free Programming_ — https://preshing.com/20120612/an-introduction-to-lock-free-programming/
3. Preshing, J. _Memory Ordering at Compile Time_ — https://preshing.com/20120625/memory-ordering-at-compile-time/
4. McKenney, P. E. _Is Parallel Programming Hard, And, If So, What Can You Do About It?_ — Appendix C (Memory Barriers)
5. Williams, A. _C++ Concurrency in Action_, 2nd ed., Chapter 5 (The C++ Memory Model)
6. Linux kernel: `include/linux/kfifo.h` — lock-free ring buffer implementation
7. LMAX Disruptor: https://lmax-exchange.github.io/disruptor/
8. `man 3 atomic` — C11 atomic operations overview
