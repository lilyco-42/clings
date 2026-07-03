/* 49_dining-philosophers-sync.c — 哲学家就餐问题：真实多线程 + 可观测死锁【标杆题】
 *
 * 任务：1. 实现 pickup()  — 按 strategy 拿筷子（naive 会触发死锁）
 *       2. 实现 putdown() — 释放两根筷子
 *       3. 实现 philosopher() — 线程函数：think → pickup → eat → putdown 循环
 *       4. 实现 watchdog()   — 进度停滞哨兵：检测死锁并打印诊断
 *       5. 实现 build_wait_cycle() — 构建等待环字符串
 *       6. 实现 coffman_check()   — 自检 Coffman 四条件是否全部命中
 *       7. 补全 main()   — 创建 barrier / 线程 / join
 *
 * 背景：五位哲学家围坐圆桌，需左右两根筷子才能进餐。每根筷子是一把
 *       pthread_mutex_t 互斥锁。strategy=naive 时五人全左手优先，
 *       会形成"持有并等待"的循环等待环 → 死锁真实发生，watchdog
 *       超时后打印等待环与 Coffman 四条件自检。
 *
 * 运行：./dining_philosophers <naive|asymmetric|ordered>
 *
 * 退出码协议（判分锚点）：
 *   0 = 全员吃完 TARGET 顿，无死锁（asymmetric / ordered 期望）
 *   2 = watchdog 触发，检测到死锁（naive 期望）
 *
 * 知识点：Coffman 四条件、pthread 互斥锁、持有并等待、循环等待、
 *         死锁检测、非对称/资源排序预防策略
 *
 * 验证：clings 分别以 naive/asymmetric/ordered 运行，断言退出码 + 关键诊断行
 *
 * 特性宏说明：-std=c11 -pedantic 下 glibc 默认不暴露 POSIX/BSD 接口
 *   (pthread_barrier_t 属 POSIX.1-2001，usleep 属 BSD)，需显式开启。
 */
#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE

#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define N 5                  /* 哲学家数量 */
#define TARGET_EAT 100       /* 每人目标进餐次数（naive 下足够稳定触发死锁） */
#define WATCHDOG_TIMEOUT 3   /* watchdog 超时秒数 */
#define THINK_US_MIN 1000    /* 思考最短（微秒） */
#define THINK_US_MAX 3000    /* 思考最长（微秒） */
#define EAT_US 1000          /* 进餐持续（微秒） */
#define GRAB_GAP_US 500      /* 拿左筷到拿右筷的间隔（放大持有并等待窗口） */

typedef enum { NAIVE, ASYMMETRIC, ORDERED } Strategy;

static const char *strategy_name(Strategy s) {
    return s == NAIVE ? "naive" : s == ASYMMETRIC ? "asymmetric" : "ordered";
}

/* 共享资源：5 根筷子 = 5 把互斥锁（Coffman 条件 1：互斥） */
static pthread_mutex_t chopstick[N];

/* 起跑门：让 5 个线程同时开始第一轮，提高 naive 死锁触发概率 */
static pthread_barrier_t start_gate;

/* 原子计数器：避免 eat_count 数据竞争（呼应 L64 _Atomic） */
static atomic_int eat_count[N];

/* 哲学家状态（仅供诊断输出，不参与同步逻辑） */
static atomic_int state[N];      /* 0=THINKING 1=HUNGRY 2=EATING */
static atomic_int holding[N];    /* 当前持有的左筷编号，-1 表示无 */

/* 全局策略（main 解析后写入，线程启动前固定，无需同步） */
static Strategy g_strategy;

/* 打印互斥锁：防止多线程 printf 输出交错 */
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;

/* 全员完成标志：watchdog 据此判断是正常结束还是死锁 */
static atomic_int all_done_flag = 0;

/* ---------- TODO 1: pickup() — 按策略拿两根筷子 ----------
 *
 * 左手筷子编号 = id，右手筷子编号 = (id + 1) % N
 *
 * 三种策略（核心：是否破坏循环等待环）：
 *
 *   NAIVE       — 全部先 lock(left) 再 lock(right)
 *                 五人同时执行 → 每人各持一根 → 互等另一根 → 循环等待 → 死锁
 *                 这是"病灶侧"，故意触发死锁供观察
 *
 *   ASYMMETRIC  — id == N-1 反过来先 lock(right) 再 lock(left)；其余同 NAIVE
 *                 打破对称性：P_{N-1} 与 P_0 在筷 0 上必有一人失败 → 环断
 *
 *   ORDERED     — 始终先 lock(min(left,right)) 再 lock(max(left,right))
 *                 资源偏序 → 不可能成环（最通用的预防策略）
 *
 * 提示：
 *   - 拿完左筷后 usleep(GRAB_GAP_US) 放大"持有并等待"窗口，让 naive 稳定死锁
 *   - 用 holding[id] = left 记录持有的左筷（watchdog 诊断用）
 *   - pthread_mutex_lock 会阻塞直到拿到，无需自己 spin
 */
static void pickup(int id) {
    int left = id;
    int right = (id + 1) % N;
    int first, second;

    /* TODO: 根据 g_strategy 计算 first / second 的拿筷顺序，然后:
     *   pthread_mutex_lock(&chopstick[first]);
     *   atomic_store(&holding[id], first);
     *   usleep(GRAB_GAP_US);            // 放大持有并等待窗口
     *   pthread_mutex_lock(&chopstick[second]);
     *   atomic_store(&state[id], 2);    // EATING
     */
#error TODO: implement pickup() per strategy.
}

/* ---------- TODO 2: putdown() — 释放两根筷子 ----------
 *
 * Coffman 条件 3（不可剥夺）的体现：筷子只能由持有者主动 unlock。
 * 释放顺序：先 unlock 后拿的那根（second），再 unlock 先拿的那根（first），
 * 这是锁的最佳实践（逆序释放，避免某些场景下的语义混乱）。
 *
 * 注意：本函数不依赖 strategy —— 无论怎么拿的，放下时两根都解锁即可。
 *       holding[id] 置 -1 表示无持有。
 */
static void putdown(int id) {
#error TODO: implement putdown(). Release both chopsticks, clear holding[id].
}

/* ---------- TODO 3: philosopher() — 线程函数 ----------
 *
 * 循环 TARGET_EAT 次：
 *   1. think()      — usleep 一个 THINK_US_MIN~MAX 的随机值
 *   2. 第一轮在 start_gate 同步起跑（让 5 人几乎同时抢筷，放大死锁概率）
 *      之后各轮不再 barrier（否则安全策略也会被强行同步成死锁）
 *   3. atomic_store(&state[id], 1)   // HUNGRY
 *   4. pickup(id)
 *   5. eat()        — usleep(EAT_US)
 *   6. putdown(id)
 *   7. atomic_fetch_add(&eat_count[id], 1)
 *
 * 线程返回 NULL。
 *
 * 提示：用 rand_r(&seed) 生成可复现的思考时长，seed 用 id 派生。
 */
static void *philosopher(void *arg) {
#error TODO: implement philosopher() thread loop.
    return NULL;
}

/* ---------- TODO 4: watchdog() — 死锁哨兵（进度检测，非固定墙钟超时）----------
 *
 * 核心思想：死锁 = 所有哲学家都无法推进 = 总进餐数不再增长。
 * 不要用 sleep(WATCHDOG_TIMEOUT) 一睡到底再判死锁——那会把"是否死锁"与"机器
 * 是否够快"混为一谈，慢机上 asymmetric/ordered 会被误判为死锁。应周期性采样
 * eat_count 之和，只有连续 WATCHDOG_TIMEOUT 秒【毫无进展】时才判定死锁。
 *
 * 参考实现：
 *   const int check_us = 200000;                                 // 200ms 采样
 *   const int stall_limit = WATCHDOG_TIMEOUT * 1000000 / check_us;
 *   int last_total = -1, stall = 0;
 *   for (;;) {
 *       usleep(check_us);
 *       if (atomic_load(&all_done_flag)) return NULL;            // 正常完成
 *       int total = 0;
 *       for (int i = 0; i < N; i++) total += atomic_load(&eat_count[i]);
 *       if (total != last_total) { last_total = total; stall = 0; }  // 有进展→重置
 *       else if (++stall >= stall_limit) {                       // 持续无进展→死锁
 *           print_deadlock_diag();
 *           exit(2);
 *       }
 *   }
 *
 * 为什么用独立线程而非 alarm()/SIGALRM？
 *   signal handler 里调 pthread 函数是已知的"会死锁的地雷"
 *   （linuxthreads FAQ：pthread 函数非 async-signal-safe）。
 *   独立线程 + exit() 是干净、可移植、无副作用的做法。
 *
 * exit(2) 是判分锚点：tests/49_*.toml 中 naive case 期望 exit_code=2。
 */
/* 前向声明：watchdog 调用，print_deadlock_diag 在下方已实现 */
static void print_deadlock_diag(void);

static void *watchdog(void *arg) {
    (void)arg;
#error TODO: implement watchdog() timeout detector.
    return NULL;
}

/* ---------- TODO 5: build_wait_cycle() — 构建等待环字符串 ----------
 *
 * 死锁时，每个哲学家 Pi 持有筷 i，等待筷 (i+1)%N（被 P_{i+1} 持有）。
 * 等待环形如：
 *   P0 → chop1 → P1 → chop2 → P2 → chop3 → P3 → chop4 → P4 → chop0 → P0
 *
 * 把上述字符串写入 out（out 至少 256 字节）。
 * 这是"循环等待"（Coffman 条件 4）的可视化。
 */
static void build_wait_cycle(char *out, size_t outsz) {
#error TODO: implement build_wait_cycle().
    (void)out; (void)outsz;
}

/* ---------- TODO 6: coffman_check() — 自检四条件 ----------
 *
 * 死锁发生当且仅当 Coffman 四条件同时成立。本函数在死锁现场
 * 把每个条件的命中情况格式化到 out：
 *
 *   Coffman check: 互斥✓ 持有等待✓ 不可剥夺✓ 循环等待✓
 *
 * 四条件说明（死锁时全部为 ✓）：
 *   1. 互斥       — 筷子是 pthread_mutex_t，同一时刻只能被一个线程持有
 *   2. 持有并等待 — pickup() 先拿左筷再等右筷，中间有 GRAB_GAP_US 窗口
 *   3. 不可剥夺   — mutex 只能由持有者 unlock，别人无法抢走
 *   4. 循环等待   — build_wait_cycle() 构造出的环
 */
static void coffman_check(char *out, size_t outsz) {
#error TODO: implement coffman_check().
    (void)out; (void)outsz;
}

/* ---------- 死锁诊断输出（已实现，调用上面的 TODO） ---------- */
static void print_deadlock_diag(void) {
    char cycle[256];
    char coffman[128];
    build_wait_cycle(cycle, sizeof cycle);
    coffman_check(coffman, sizeof coffman);

    pthread_mutex_lock(&print_lock);
    printf("\n!!! DEADLOCK DETECTED !!!\n");
    printf("Held: ");
    for (int i = 0; i < N; i++) {
        int h = atomic_load(&holding[i]);
        printf("P%d←chop%d  ", i, h < 0 ? i : h);
    }
    printf("\nWait: ");
    for (int i = 0; i < N; i++) {
        printf("P%d→chop%d  ", i, (i + 1) % N);
    }
    printf("\nCycle: %s\n", cycle);
    printf("%s\n", coffman);
    printf("exit code: 2\n");
    fflush(stdout);
    pthread_mutex_unlock(&print_lock);
}

/* ---------- TODO 7: main() — 初始化与线程编排 ----------
 *
 * 1. 解析 argv[1] → g_strategy（默认 NAIVE）
 * 2. 打印头部 banner（包含 strategy=xxx，判分用）
 * 3. 初始化 N 把 mutex（PTHREAD_MUTEX_DEFAULT 即可）
 * 4. pthread_barrier_init(&start_gate, NULL, N)
 * 5. 初始化 eat_count / state / holding
 * 6. 创建 watchdog 线程（detached，它自己会 exit）
 * 7. 创建 N 个 philosopher 线程
 * 8. pthread_join 全部 philosopher
 * 9. atomic_store(&all_done_flag, 1)  ← 告诉 watchdog 正常结束
 * 10. 打印 Final Stats（含每个 Pi ate TARGET 次 + "all philosophers finished"）
 * 11. 销毁 mutex / barrier，return 0
 *
 * 提示：watchdog 用 detached 创建，避免 main 退出时 join 它（它会先 exit(2)）。
 */
int main(int argc, char **argv) {
#error TODO: implement main() thread orchestration.
    (void)argc; (void)argv;
    return 0;
}
