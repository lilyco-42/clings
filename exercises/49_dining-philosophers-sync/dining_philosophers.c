/* 49_dining-philosophers-sync.c — 哲学家就餐问题：状态机模拟 + 非对称防死锁【标杆题】
 *
 * 任务：1. 补全 init() 中的定时器初始化
 *       2. 实现 can_eat() — 非对称筷子检测
 *       3. 实现 pickup()  — 拿筷子
 *       4. 实现 putdown() — 放筷子
 *       5. 实现 all_done() — 判定全员完成
 *       6. 实现 print_state() — 格式化输出
 *       7. 补全 main() 中的三阶段模拟循环
 *
 * 背景：五位哲学家围坐圆桌，需左右两根筷子才能进餐。
 *       全左手优先 → 循环等待 → 死锁。
 *       让 P4 右手优先打破循环等待，杜绝死锁。
 *
 * 知识点：死锁四条件、循环等待、非对称策略、状态机模拟
 *
 * 验证：srand(42) 固定种子 → make test 比对 expected_output.txt
 */
#include <stdio.h>
#include <stdlib.h>

#define N 5              /* 哲学家数量 */
#define TARGET_EAT 3     /* 每人吃满 N 次后结束 */
#define MAX_ROUNDS 200   /* 最大轮次（安全上限） */
#define THINK_TIME_MIN 2 /* 思考最短持续时间（轮） */
#define THINK_TIME_MAX 5 /* 思考最长持续时间（轮） */
#define EAT_TIME 1       /* 进餐持续时间（轮） */

typedef enum { THINKING, HUNGRY, EATING } State;

State state[N];   /* 每位哲学家的当前状态 */
int chopstick[N]; /* -1 = 空闲，否则 = 持有者的 id */
int eat_count[N]; /* 每位已完成进餐次数 */
int timer[N];     /* 当前状态剩余轮数 */

/* ---------- 初始化 ---------- */
static void init(void) {
    srand(42); /* 固定种子 → 输出可复现 */
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        chopstick[i] = -1;
        eat_count[i] = 0;
#error TODO: Finish this exercise. Run "clings hint" for help.
        /* 用 rand() 生成 THINK_TIME_MIN ~ THINK_TIME_MAX 的随机思考时间
         * 提示：rand() % 范围大小 + 最小值 */
    }
}

/* ---------- 检查哲学家 id 是否可以进餐 — 非对称策略 ---------- */
static int can_eat(int id) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 记住筷子的编号规则：
     *   哲学家 i 的左手是 筷 [i]，右手是 筷 [(i+1)%N]
     *
     * P0 ~ P3  左手优先：检查筷 [左手] 空闲 && 筷 [右手] 空闲
     * P4       右手优先：检查筷 [右手] 空闲 && 筷 [左手] 空闲
     *   (为什么 P4 要反过来？—— 打破循环等待)
     *
     * 两根都空闲返回 1，否则返回 0 */
}

/* ---------- 拿起筷子 — 非对称顺序 ---------- */
static void pickup(int id) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 左手 = id，右手 = (id+1)%N
     *
     * P0 ~ P3: 先拿左手，再拿右手
     * P4:      先拿右手，再拿左手
     *
     * chopstick[手] = id 表示被 id 号哲学家持有 */
}

/* ---------- 放下筷子 ---------- */
static void putdown(int id) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 将左右两根筷子都标记为 -1（空闲）*/
}

/* ---------- 所有人都吃满了吗？---------- */
static int all_done(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 遍历所有哲学家：
     *   任何一位 eat_count[i] < TARGET_EAT → 返回 0
     *   全部达标 → 返回 1 */
}

/* ---------- 打印当前状态 ---------- */
static void print_state(int round) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 输出格式示例 (round 用 %02d 补零):
     *   02 | P0:T P1:H P2:T P3:T P4:H
     *
     * 状态字符映射：THINKING→'T', HUNGRY→'H', EATING→'E'
     * 注意每项之间有空格，末尾换行 */
}

/* ---------- 主模拟循环 ---------- */
int main(void) {
    init();

    printf("=== Dining Philosophers ===\n");
    printf("N=5, asymmetric (P4 right-first), target=%d meals each\n", TARGET_EAT);
    printf("T=Thinking H=Hungry E=Eating\n\n");

    int round = 0;
    while (round < MAX_ROUNDS) {
        print_state(round);

#error TODO: Finish this exercise. Run "clings hint" for help.
        /* ═══ 阶段 1: EATING 哲学家进餐结束 ═══
         * 遍历所有哲学家，若状态 == EATING:
         *   timer[i] 减 1
         *   若 timer[i] 归零 → putdown + eat_count++ → 状态 = THINKING
         *   → 用 rand() 设置新的思考时间（范围同 init）
         *
         * ═══ 退出检查 ═══
         * 若 all_done() → break
         *
         * ═══ 阶段 2: HUNGRY 哲学家尝试进餐 ═══
         * 遍历所有哲学家，若状态 == HUNGRY 且 can_eat(i):
         *   → pickup + 状态 = EATING + timer = EAT_TIME
         *
         * ═══ 阶段 3: THINKING 哲学家计时到 → 变饿 ═══
         * 遍历所有哲学家，若状态 == THINKING:
         *   timer[i] 减 1；若归零 → 状态 = HUNGRY
         *
         * 注意：三个阶段顺序有讲究——
         *   先"吃完放筷子", 再"抢筷子", 最后"思考变饿" */

        round++;
    }

    printf("\n=== Final Stats ===\n");
    for (int i = 0; i < N; i++) printf("P%d ate %d time(s)\n", i, eat_count[i]);
    printf("Total rounds: %d\n", round);
    return 0;
}
