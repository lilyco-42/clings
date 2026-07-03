## Lesson 49 — 哲学家就餐问题：真实多线程 + 可观测死锁【标杆题】

### 课程任务

用 **POSIX 线程 (pthreads)** 真实模拟五位哲学家围坐圆桌就餐：每人需要左右两根筷子才能进餐，每根筷子是一把 `pthread_mutex_t` 互斥锁。程序支持三种策略 `naive` / `asymmetric` / `ordered`：

- `naive` — 全部先拿左筷再拿右筷，**故意触发死锁**，watchdog 超时检测并打印诊断
- `asymmetric` — 让一位哲学家反过来先拿右筷，**打破循环等待**
- `ordered` — 始终先拿编号小的筷子，**资源偏序预防**

每人吃满 **100 次**后模拟结束（`naive` 会在死锁时提前由 watchdog 终止）。

实现 `pickup()`、`putdown()`、`philosopher()`、`watchdog()`、`build_wait_cycle()`、`coffman_check()` 和 `main()` 中的线程编排。

验证：`make && ./dining_philosophers naive` 应在 3 秒内输出 `DEADLOCK DETECTED` 并以退出码 2 退出；`asymmetric` / `ordered` 应正常完成并以退出码 0 退出。

```
$ ./dining_philosophers naive
=== Dining Philosophers (strategy=naive) ===
N=5, target=100 meals each, watchdog=3s
...
!!! DEADLOCK DETECTED !!!
Cycle: P0 → chop1 → P1 → chop2 → P2 → chop3 → P3 → chop4 → P4 → chop0 → P0
Coffman check: 互斥✓ 持有等待✓ 不可剥夺✓ 循环等待✓
exit code: 2
```

### 前置知识：为什么必须用真实多线程

单线程模拟里，"检查两根筷子同时空闲才拿"是一个**原子动作**——根本不存在"持有一根、等待另一根"的中间态。这意味着 Coffman 死锁四条件里的**"持有并等待"无法在代码中表达**，循环等待环无法构造，学员永远看不到死锁真实发生。

真实多线程下，`pthread_mutex_lock` 会**真正阻塞**调用线程，"持有左筷等右筷"的窗口真实存在 → 循环等待环可构造、可观测、可检测。这才是教死锁的正道。

学术上，单核机器的并发本质就是**交错执行 (interleaving)**——线程调度器在多个线程间快速切换，模拟"同时发生"的效果。多核机器则是真正的物理并行。无论哪种，共享资源竞争都是真实存在的，这正是死锁问题的温床。

### 前置知识：Coffman 死锁四条件

死锁发生当且仅当以下四个条件**同时成立**（缺一不可）：

```
    (1) 互斥 Mutex        — 资源同一时刻只能被一个线程持有
    (2) 持有并等待 H & W   — 持有已有资源 + 等待新资源
    (3) 不可剥夺 No Preempt — 资源只能由持有者主动释放
    (4) 循环等待 Circular   — 存在一条资源等待环
```

在本题中，四个条件的代码化身：

```
┌──────────────┬─────────────────────────────────────────────┐
│ Coffman 条件  │ 代码化身                                    │
├──────────────┼─────────────────────────────────────────────┤
│ 互斥         │ pthread_mutex_t — 同一时刻只能被一个线程持有 │
│ 持有并等待    │ pickup() 先 lock(left) 再 lock(right)，     │
│              │ 中间 usleep(GRAB_GAP_US) 放大等待窗口        │
│ 不可剥夺      │ mutex 只能由持有者 unlock，别人无法抢走      │
│ 循环等待      │ build_wait_cycle() 构造的 P→chop→P→... 环   │
└──────────────┴─────────────────────────────────────────────┘
```

**naive 策略下四条件全部命中 → 死锁必然发生。** 预防策略的核心就是破坏其中至少一个条件（本题破坏第 4 条"循环等待"）。

### 前置知识：pthread 互斥锁基础

```c
pthread_mutex_t chopstick;          /* 声明 */
pthread_mutex_init(&chopstick, NULL);   /* 初始化（默认属性）*/
pthread_mutex_lock(&chopstick);     /* 加锁：若已被占则阻塞等待 */
/* ... 临界区 ... */
pthread_mutex_unlock(&chopstick);   /* 解锁：唤醒一个等待者 */
pthread_mutex_destroy(&chopstick);  /* 销毁 */
```

`pthread_mutex_lock` 是**阻塞调用**：若锁已被其他线程持有，当前线程会被操作系统挂起（进入等待队列），直到锁被释放再被唤醒。这个"阻塞"正是死锁的物理表现——线程永久挂在 `lock()` 里出不来。

### 算法深度剖析

#### 筷子编号与哲学家布局

```
             P4
       筷4       筷0
    P3                P0
       筷3       筷1
    P2     筷2     P1

    哲学家 i 的左筷 = i，右筷 = (i+1) % 5
    P0: 左=筷0 右=筷1
    P1: 左=筷1 右=筷2
    P2: 左=筷2 右=筷3
    P3: 左=筷3 右=筷4
    P4: 左=筷4 右=筷0
```

#### 三种拿筷策略对比

```
NAIVE（病灶侧 — 故意死锁）:
    所有 Pi: lock(筷i); usleep; lock(筷(i+1)%5)
    五人同时执行 → 每人各持一根 → 互等另一根 → 循环等待环 → 死锁！

        P0: 持筷0 等筷1 ─┐
        P1: 持筷1 等筷2  │
        P2: 持筷2 等筷3  ├─ 循环等待环！
        P3: 持筷3 等筷4  │
        P4: 持筷4 等筷0 ─┘
        → 死锁！无人能进餐，watchdog 3 秒后检测并退出

ASYMMETRIC（非对称 — 打破循环等待）:
    P0~P3: 先左后右（同 NAIVE）
    P4:    先右(筷0)后左(筷4)   ← 关键：反转一人

        P4 和 P0 都先抢筷0 → 只有一人能拿到
        → 等待环在筷0处必然断开 → 死锁永远不发生 ✓

ORDERED（资源排序 — 偏序预防）:
    所有 Pi: 先 lock(min(左,右)) 再 lock(max(左,右))

        资源存在全局偏序 → 不可能形成环 → 死锁永远不发生 ✓
        这是最通用的死锁预防策略，适用于任意资源图
```

#### 死锁触发保障：barrier 同步起跑

naive 策略下，若 5 个线程启动有先后，可能某个线程已吃完放下筷子，死锁就不成立了。为**稳定触发死锁**（消除 flaky），用 `pthread_barrier_t` 让 5 个线程第一轮同时起跑：

```
┌─────────────────────────────────────────────────┐
│  start_gate = pthread_barrier_init(NULL, NULL, 5)│
│                                                 │
│  线程 P0: barrier_wait ──┐                      │
│  线程 P1: barrier_wait ──┤                      │
│  线程 P2: barrier_wait ──┼──→ 5 人到齐，同时放行│
│  线程 P3: barrier_wait ──┤                      │
│  线程 P4: barrier_wait ──┘                      │
│                                                 │
│  放行瞬间 5 人几乎同时 lock(左筷) → 各持一根     │
│  + GRAB_GAP_US 窗口放大 → 循环等待环必然闭合    │
└─────────────────────────────────────────────────┘
```

配合 `pickup()` 中拿完左筷后的 `usleep(GRAB_GAP_US)`（放大"持有并等待"窗口），naive 策略**每次必死锁**，无 flaky。

#### watchdog 死锁检测（进度检测，而非固定墙钟超时）

死锁的本质不变量是**所有哲学家都无法推进**——总进餐数 `Σ eat_count` 不再增长。因此 watchdog 不应"睡满 3 秒就判死锁"（那会把"是否死锁"和"机器是否够快"混为一谈，慢机/超卖 CI 上 `asymmetric`/`ordered` 会被**误报**死锁）；而应周期性采样进度，仅当**连续 `WATCHDOG_TIMEOUT` 秒毫无进展**时才判定死锁：

```c
watchdog 线程:
  last_total = -1; stall = 0;
  for (;;) {
      usleep(200ms);                                 /* 周期性采样 */
      if (all_done_flag) return;                     /* 正常完成 */
      total = Σ eat_count[i];
      if (total != last_total) { last_total = total; stall = 0; }  /* 有进展→重置 */
      else if (++stall >= WATCHDOG_TIMEOUT 秒对应的次数) {
          print_deadlock_diag();                     /* 打印等待环 + Coffman 自检 */
          exit(2);                                   /* 退出码 2 = 死锁 */
      }
  }
```

- `naive`：合围后无人能放下筷子 → 总数停滞 → 约 3 秒后检出死锁（与"3 秒内 exit(2)"的期望一致）。
- `asymmetric`/`ordered`：即使很慢也在持续进餐 → 总数不断增长 → 永不误报，**根除 flaky**。

**为什么用独立线程而非 `alarm()`/`SIGALRM`？** 因为 signal handler 里调用 pthread 函数是**已知的死锁地雷**——linuxthreads FAQ 明确指出 pthread 函数非 async-signal-safe，在 signal handler 里调用会导致程序自身死锁。独立线程 + `usleep()` 轮询 + `exit()` 是干净、可移植、无副作用的做法。

### 完整死锁过程逐轮追踪（naive 策略）

下表展示 naive 策略下从起跑到死锁的微观过程（时间线）：

| 时刻 | P0 | P1 | P2 | P3 | P4 | 事件 |
| ---- | -- | -- | -- | -- | -- | ---- |
| t0 | barrier | barrier | barrier | barrier | barrier | 5 人在起跑门等待 |
| t1 | 持筷0 | 持筷1 | 持筷2 | 持筷3 | 持筷4 | barrier 放行，5 人同时 lock 左筷 |
| t2 | usleep | usleep | usleep | usleep | usleep | GRAB_GAP_US 放大持有并等待窗口 |
| t3 | 等筷1 | 等筷2 | 等筷3 | 等筷4 | 等筷0 | 5 人尝试 lock 右筷，全被阻塞 |
| t4 | 阻塞 | 阻塞 | 阻塞 | 阻塞 | 阻塞 | 循环等待环闭合，无人能前进 |
| ... | ... | ... | ... | ... | ... | 永久阻塞 |
| t0+3s | — | — | — | — | — | watchdog 超时，打印诊断，exit(2) |

**Coffman 四条件此时全部命中**：互斥（mutex）✓ 持有并等待（持左等右）✓ 不可剥夺（mutex 不能被抢）✓ 循环等待（P0→筷1→P1→筷2→...→P0）✓。

### 安全策略运行追踪（asymmetric / ordered）

| 时刻 | P0 | P1 | P2 | P3 | P4 | 事件 |
| ---- | -- | -- | -- | -- | -- | ---- |
| t0 | barrier | barrier | barrier | barrier | barrier | 起跑门等待 |
| t1 | 持筷0 | 持筷1 | 持筷2 | 持筷3 | 抢筷0失败 | P4 先抢筷0，但 P0 已拿到 |
| t2 | 进餐 | 进餐 | 进餐 | 进餐 | 等待 | P0~P3 进餐，P4 等筷0释放 |
| t3 | 放筷 | 放筷 | 放筷 | 放筷 | 持筷0 | P0 释放筷0，P4 拿到 |
| t4 | 思考 | 思考 | 思考 | 思考 | 进餐 | P4 进餐 |
| ... | ... | ... | ... | ... | ... | 轮流进餐，无死锁 |
| 最终 | 100次 | 100次 | 100次 | 100次 | 100次 | 全员完成，exit(0) |

关键：P4 反转拿筷顺序后，与 P0 在筷0上形成**竞争**——必有一人失败，失败者不持有任何资源，等待环在此断开。

### 边界情况分析

| 场景 | 期望行为 | 原因 |
| ---- | -------- | ---- |
| naive 单次运行 | 3 秒内死锁，exit(2) | barrier+gap 保证循环等待闭合 |
| naive 高并发核数 | 仍死锁 | 死锁与核数无关，只与拿筷顺序有关 |
| asymmetric 单核 | 不死锁 | 非对称破坏循环等待，与调度无关 |
| ordered 线程数变化 | 不死锁 | 资源偏序是数学保证 |
| 慢机/超卖 CI 上 safe 跑得慢 | 不误报（进度检测的关键收益） | 只要 `Σeat_count` 仍在增长就不判死锁，与绝对耗时无关 |
| 无进展窗口设太长 | naive 卡太久 | 死锁后要等更久（窗口内无进展累计）才退出 |
| TARGET_EAT 太小 | naive 可能不死锁 | 轮次少，错开起跑概率增大 |
| 不用 barrier | naive 可能 flaky | 线程启动有先后，错开拿筷 |
| eat_count 不用原子 | 数据竞争 | 多线程并发写共享变量（UB） |

### 对比表：三策略量化

| 维度 | naive | asymmetric | ordered |
| ---- | ----- | ---------- | ------- |
| 死锁 | **必然发生** | 永不发生 | 永不发生 |
| 破坏哪个 Coffman 条件 | 无（全保留） | 循环等待 | 循环等待 |
| 机制 | 无防御 | 打破对称性 | 资源偏序 |
| 退出码 | 2 | 0 | 0 |
| 公平性 | 全饿死 | 轮询公平 | 轮询公平 |
| 通用性 | — | 仅环形拓扑 | 任意资源图 |
| 代码复杂度 | 最简 | 多一个 if/else | min/max 分支 |
| 实际工程适用 | 警示用 | 特定场景 | **最常用** |

### 对比表：单线程模拟 vs 真实多线程

| 维度 | 单线程状态机模拟 | 真实多线程（本题） |
| ---- | ---------------- | ------------------ |
| 资源竞争 | 无（顺序访问） | 真实竞争 |
| 持有并等待 | 无法表达 | pickup() 中间态真实 |
| 死锁可观测 | 永不发生 | naive 必然发生 |
| Coffman 四条件 | 只能背诵 | 代码可映射、可自检 |
| 输出确定性 | 完全确定 | 非确定（靠不变量判分） |
| 教学价值 | 工程化/格式化 | **并发思维核心** |
| 依赖 | 无 | pthreads（系统库） |

### 常见错误与陷阱

| 错误 | 后果 | 正确做法 |
| ---- | ---- | -------- |
| watchdog 用 `alarm()`+`SIGALRM` | signal handler 调 pthread 自身死锁 | 用独立 watchdog 线程 + `exit()` |
| `eat_count` 用普通 `int` | 数据竞争（UB），计数错乱 | 用 `_Atomic int` |
| 不用 `print_lock` 保护 printf | 输出交错，无法判分 | 所有输出用 mutex 包裹 |
| naive 不加 barrier | 死锁 flaky（时锁时不锁） | `pthread_barrier_wait` 同步起跑 |
| naive 不加 GRAB_GAP_US | 拿两根筷太快，可能错过死锁窗口 | 拿完左筷 usleep 放大窗口 |
| putdown 顺序错（先放先拿的） | 语义混乱，某些场景难推理 | 逆序释放（先放后拿的） |
| 忘了 `pthread_join` | main 提前退出，线程被杀 | join 所有 philosopher |
| Makefile 漏 `-pthread` | 链接失败（undefined reference） | 编译期 + 链接期都加 `-pthread` |
| `holding[]` 不用原子 | 诊断输出数据竞争 | 用 `_Atomic int` |

### 重要知识点

- **Coffman 四条件**：互斥、持有等待、不可剥夺、循环等待——四者齐备才死锁，预防只需破坏一个
- **循环等待**：naive 下五人各持一筷互等，形成 P0→筷1→P1→...→P0 的环，是死锁的核心结构
- **非对称策略**：让一个进程改变资源请求顺序，打破对称性 → 循环等待环断开
- **资源排序**：始终按全局偏序获取资源，是**最通用**的死锁预防策略，适用于任意资源图
- **持有并等待**：`pickup()` 先 lock 左筷再 lock 右筷，中间的等待窗口是 Coffman 条件 2 的代码化身
- **pthread_barrier**：同步起跑门，让多线程在同一瞬间开始，提高死锁触发概率（消除 flaky）
- **watchdog 模式**：独立线程超时检测，避免 signal+pthread 的地雷，是并发程序可观测性的常用手段
- **`_Atomic` 防数据竞争**：共享计数器必须原子访问，呼应 L64 无锁编程
- **`-pthread` 工程化**：编译期定义 `_REENTRANT`，链接期链接 pthread 库，两处缺一不可
- **退出码协议**：0=正常，2=死锁，作为自动化判分的不变量锚点

### 课堂讨论

1. **为什么 naive 一定会死锁？barrier 和 GRAB_GAP_US 各起什么作用？**
   barrier 让 5 个线程同时开始第一轮，保证 5 人几乎同一瞬间执行 `lock(左筷)`；GRAB_GAP_US 在拿完左筷后制造一个等待窗口，让其他线程有机会也拿到各自的左筷。两者配合使"每人各持一根"的局面稳定形成，循环等待环必然闭合。没有 barrier，线程启动有先后，可能某线程已吃完释放，死锁就不成立。

2. **asymmetric 为什么选 P4 反转，选 P0 行不行？**
   选谁都行，关键是**打破对称性**——只要至少一个进程的拿筷顺序与其他人不同，循环等待环就在该进程处断开。本题选 P4（编号最大）只是约定俗成。数学上，任选一个进程反转都能破坏环。

3. **ordered 策略为什么能保证无死锁？**
   资源排序策略要求所有进程按统一的全局偏序获取资源。本题中"先拿编号小的筷子"意味着：若 P_i 持有筷 a 等待筷 b，则必有 a < b。循环等待环要求存在 P_0→...→P_k→P_0 的链，每条边 P_x 持有 a_x 等待 a_{x+1}，则 a_0 < a_1 < ... < a_k < a_0，矛盾（严格递增不可能成环）。这是**数学证明**，与调度无关。

4. **为什么不能用 `alarm()`/`SIGALRM` 做超时检测？**
   `alarm()` 注册的 signal handler 在任意线程被异步中断执行。pthread 函数（如 `pthread_mutex_lock`）**不是 async-signal-safe** 的——在 signal handler 里调用它们会导致程序自身死锁或未定义行为。linuxthreads FAQ 明确警告"尽量少混用信号和线程"。独立 watchdog 线程用普通 `sleep()` + `exit()` 完全避开这个地雷。

5. **`eat_count` 为什么必须用 `_Atomic`？普通 `int` 会怎样？**
   5 个线程并发执行 `eat_count[i]++`，这其实是"读-改-写"三步操作，不是原子的。两个线程同时读到旧值、各自加 1、写回，会导致丢失一次更新（数据竞争，C 标准定义为未定义行为）。`_Atomic int` 保证 `++` 是原子的、可见的。这也为 L64 的无锁编程埋下伏笔。

6. **真实多线程输出非确定，怎么自动化判分？**
   不依赖逐行精确 diff，而是检查**结构化不变量**：退出码（0=正常/2=死锁）、关键诊断行是否存在（`DEADLOCK DETECTED`、`Cycle:`、`Coffman check`、`all philosophers finished`）、禁止行是否缺席（safe 策略不能出现 `DEADLOCK`）。clings 的判分器支持 `stdout_contains` / `stdout_not_contains` / `exit_code` 组合断言，专门适配非确定输出。

### 后续衔接

- **Lesson 50**：停等协议（网络可靠传输）—— 单线程状态机，与并发无关，但同样涉及"超时重传"的 watchdog 思想
- **Lesson 56**：向量时钟（分布式系统）—— 并发思维的延伸，从单机多线程到分布式 happens-before
- **Lesson 64**：无锁环形缓冲区（C11 `_Atomic`）—— 并发进阶，用原子操作替代互斥锁，从"悲观同步"到"乐观无锁"的自然递进
- **高阶延伸**：真实生产环境的死锁检测工具（ThreadSanitizer）、Banker 算法（死锁避免）、Chandy/Misra 方案（消息传递解法）

### 参考资料

- Dijkstra, E.W. "Hierarchical ordering of sequential processes" (1971) — 哲学家就餐问题原论文
- Coffman, E.G. et al. "System Deadlocks" (1971) — Coffman 四条件首次系统化论述
- 《操作系统概念》(Silberschatz) 第 7 章 — 死锁预防/避免/检测/恢复
- 《现代操作系统》(Tanenbaum) 第 6 章 — 死锁
- POSIX.1-2017 standard — `pthread_mutex_*`, `pthread_barrier_*`, `pthread_create/join`
- linuxthreads FAQ — signal 与线程混用的陷阱
- Wikipedia: Dining philosophers problem — https://en.wikipedia.org/wiki/Dining_philosophers_problem
- Wikipedia: Deadlock — https://en.wikipedia.org/wiki/Deadlock_(computer_science)
- OSTEP (Operating Systems: Three Easy Pieces) — https://pages.cs.wisc.edu/~remzi/OSTEP/
