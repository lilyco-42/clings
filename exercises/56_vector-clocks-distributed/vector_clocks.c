/* 56_vector-clocks-distributed.c — 向量时钟：分布式系统中的偏序关系【标杆题】
 *
 * 任务：1. 补全 init_clocks() — 初始化所有节点的向量时钟为 0
 *       2. 补全 send_event()  — 发送方递增自己的时钟，输出时钟快照
 *       3. 补全 recv_event()  — 接收方合并向量时钟 (逐元素取 max) 后递增自己
 *       4. 补全 local_event() — 本地事件递增自己的时钟
 *       5. 补全 print_clocks() — 格式化输出所有节点的向量时钟
 *       6. 补全 happens_before() — 判断两个向量时钟的 Happens-Before 关系
 *       7. 补全 main() 事件循环 — 按固定序列执行 6 个事件
 *
 * 背景：在分布式系统中，物理时钟不可靠 (NTP 误差、时钟漂移)。
 *       Lamport 逻辑时钟能捕获因果关系但无法检测并发——
 *       若 C(a) < C(b) 不能推出 a → b。
 *       向量时钟通过为每个节点维护一个计数器向量，
 *       精确刻画偏序 (partial order) 关系。
 *
 * 固定事件序列 (3 节点 P0/P1/P2):
 *   E1: P0 SEND to P1
 *   E2: P1 RECV from P0
 *   E3: P1 SEND to P2
 *   E4: P2 RECV from P1
 *   E5: P2 SEND to P0
 *   E6: P0 RECV from P2
 *
 * 知识点：向量时钟、Happens-Before、偏序关系、因果关系追踪
 *
 * 验证：make 构建后 clings run 56 / clings check 56（查看期望：clings tests 56）
 */
#include <stdio.h>

#define N_NODES 3
#define N_EVENTS 6

typedef struct {
    int id;             /* 节点编号 0/1/2 */
    int clock[N_NODES]; /* 向量时钟 [P0, P1, P2] */
} Node;

typedef struct {
    int type;         /* 0=send, 1=recv, 2=local */
    int from;         /* 发送方 (recv 时使用) */
    int to;           /* 接收方 / 执行节点 */
    const char *desc; /* 事件描述 */
} Event;

static Node nodes[N_NODES];

/* ---------- 初始化所有节点的向量时钟为 0 ---------- */
static void init_clocks(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 遍历所有节点：
     *   - 设置 nodes[i].id = i
     *   - 将 clock 数组所有元素设为 0
     * 提示：双重循环，外层遍历节点，内层遍历 clock 分量 */
}

/* ---------- 本地事件：递增自己分量的时钟 ---------- */
static void local_event(int node_id) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 将 nodes[node_id].clock[node_id] 加 1 */
}

/* ---------- 发送事件：递增自己的时钟，输出快照到 msg_clock ---------- */
static void send_event(int from, int to, int *msg_clock) {
    (void)to; /* 保留参数以保持接口对称 */
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 1. 递增 nodes[from].clock[from]
     * 2. 将 nodes[from] 的整个 clock 数组复制到 msg_clock
     *    (这个消息快照将随消息发送给接收方)
     * 提示：用 for 循环复制 N_NODES 个分量 */
}

/* ---------- 接收事件：合并向量时钟后递增自己的分量 ---------- */
static void recv_event(int to, const int *msg_clock) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 1. 逐元素取 max:
     *    for i in 0..N_NODES-1:
     *       if msg_clock[i] > nodes[to].clock[i]:
     *          nodes[to].clock[i] = msg_clock[i]
     *
     * 2. 递增自己的分量：nodes[to].clock[to]++
     *
     * 为什么先合并再递增？—— 接收事件本身也是一次"发生"，
     * 它必须发生在消息到达之后。 */
}

/* ---------- 打印所有节点的向量时钟 ---------- */
static void print_clocks(int event_num, const char *desc) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 输出格式：
     *   E<N>: <事件描述>
     *     P0: [c0, c1, c2]
     *     P1: [c0, c1, c2]
     *     P2: [c0, c1, c2]
     *
     * 示例：
     *   E1: P0 SEND to P1
     *     P0: [1, 0, 0]
     *     P1: [0, 0, 0]
     *     P2: [0, 0, 0]
     *
     * 注意：printf 中 %d 打印整数，每行末尾有换行 */
}

/* ---------- Happens-Before 判断 ---------- */
static int happens_before(const int *a, const int *b) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 向量时钟的偏序关系：
     *
     * a → b (a Happens-Before b) 当且仅当：
     *   1. 对所有 k: a[k] <= b[k]  (a 的每个分量都不大于 b)
     *   2. 存在 k: a[k] < b[k]     (至少一个分量严格小于)
     *
     * 返回值：1 表示 a→b, 0 表示没有 Happens-Before 关系
     *
     * 提示：
     *   - 遍历所有分量，如果发现 a[k] > b[k] 立即返回 0
     *   - 同时跟踪是否发现严格小于的分量
     *   - 最后返回"发现严格小于"的标志
     *
     * 三种可能的结果：
     *   a→b:  所有分量 a[k]<=b[k] 且至少一个 a[k]<b[k]
     *   b→a:  所有分量 b[k]<=a[k] 且至少一个 b[k]<a[k]
     *   并发：既非 a→b 也非 b→a (存在 a[i]<b[i] 且 a[j]>b[j]) */
}

/* ---------- 辅助：打印 Happens-Before 测试结果 ---------- */
static void test_hb(const char *label, const int *c1, const int *c2, int expected) {
    int result = happens_before(c1, c2);
    printf("  %s: %s (expected %s)\n", label, result ? "YES" : "NO", expected ? "YES" : "NO");
}

/* ---------- 主流程 ---------- */
int main(void) {
    /* 保存每个事件后各节点时钟的快照 (用于 HB 测试) */
    int snapshots[N_EVENTS + 1][N_NODES][N_NODES];

    /* 固定事件序列: 一条 send→recv 因果链 P0→P1→P2→P0 */
    Event events[N_EVENTS] = {
        {0, 0, 1, "P0 SEND to P1"},   {1, 0, 1, "P1 RECV from P0"}, {0, 1, 2, "P1 SEND to P2"},
        {1, 1, 2, "P2 RECV from P1"}, {0, 2, 0, "P2 SEND to P0"},   {1, 2, 0, "P0 RECV from P2"},
    };

    init_clocks();

    printf("=== Vector Clocks: 3 Nodes (P0, P1, P2) ===\n\n");

    /* 保存初始快照 */
    for (int i = 0; i < N_NODES; i++)
        for (int j = 0; j < N_NODES; j++) snapshots[0][i][j] = nodes[i].clock[j];

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* ═══ 事件循环：遍历 6 个事件 ═══
     *
     * 在循环外声明"在途消息"：int msg_clock[N_NODES] = {0};
     * (SEND 写入发送时的快照，其后配对的 RECV 读取它)
     *
     * for e = 0; e < N_EVENTS; e++:
     *
     *   根据 events[e].type 分发：
     *
     *   case 0 (SEND):
     *     调用 send_event(events[e].from, events[e].to, msg_clock);
     *     (send_event 内部先自增发送方分量，再把快照写入 msg_clock)
     *
     *   case 1 (RECV):
     *     调用 recv_event(events[e].to, msg_clock);
     *     (直接用上一条 SEND 写入 msg_clock 的快照：逐分量 max 后自增自身)
     *
     *   case 2 (LOCAL):
     *     调用 local_event(events[e].to);
     *
     *   每次事件后：
     *     调用 print_clocks(e+1, events[e].desc);
     *     保存快照到 snapshots[e+1][i][j]
     *
     * 关键（向量时钟标准语义）：
     *   - 消息携带的是发送方【发送时】的时钟快照(由 SEND 写入 msg_clock)，
     *     RECV 直接使用它——而不是去读发送方"当前"的时钟(那等价于共享内存)。
     *   - 快照保存用双重循环复制 nodes[i].clock[j] */

    /* ═══ Happens-Before 测试 ═══ */
    printf("\n=== Happens-Before Tests ===\n\n");
    printf("Rule: A→B iff for all k: A[k]<=B[k] AND exists k: A[k]<B[k]\n\n");

    /* 取各事件后相关节点的时钟快照 */
    int *c1 = snapshots[1][0]; /* P0 after E1 */
    int *c2 = snapshots[2][1]; /* P1 after E2 */
    int *c3 = snapshots[3][1]; /* P1 after E3 */
    int *c4 = snapshots[4][2]; /* P2 after E4 */
    int *c5 = snapshots[5][2]; /* P2 after E5 */
    int *c6 = snapshots[6][0]; /* P0 after E6 */

    test_hb("E1→E2 (send→recv)", c1, c2, 1);
    test_hb("E2→E3 (same node)", c2, c3, 1);
    test_hb("E1→E3 (transitive)", c1, c3, 1);
    test_hb("E3→E4 (send→recv)", c3, c4, 1);
    test_hb("E3→E5 (transitive)", c3, c5, 1);
    test_hb("E1→E5 (chain)", c1, c5, 1);
    test_hb("E5→E1 (reverse)", c5, c1, 0);
    test_hb("E3→E6 (full chain)", c3, c6, 1);
    test_hb("E4→E6 (send→recv)", c4, c6, 1);

    return 0;
}
