/* 50_rdt-stop-and-wait.c — 可靠数据传输：停等协议【标杆题】
 *
 * 任务：1. 实现 log_event()   — 日志格式化输出
 *       2. 实现 sender_send() — 发送方发包
 *       3. 实现 sender_ack()  — 发送方处理 ACK / 超时
 *       4. 实现 receiver_recv() — 接收方处理包
 *       5. 实现 init()        — 初始化随机种子和变量
 *       6. 补全 main()         — 停等协议主循环
 *
 * 背景：真实网络中，数据包可能丢失、损坏、乱序。停等协议
 *       (Stop-and-Wait) 是可靠传输的"Hello World"——
 *       发一个包，等一个 ACK，超时重传，序号 0/1 交替。
 *
 * 知识点：序号空间、超时重传、ACK 确认、不可靠信道模拟
 *
 * 验证：srand(42) 固定种子 → make test 比对 expected_output.txt
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_COUNT 4      /* 待发送消息数量 */
#define MAX_MSG_LEN 16   /* 每则消息最大长度 */
#define LOSS_RATE 25     /* 丢包概率 (%) */
#define CORRUPT_RATE 10  /* 损坏概率 (%) */
#define ACK_LOSS_RATE 10 /* ACK 丢失概率 (%) */
#define MAX_SENDS 30     /* 最大发送次数（安全上限）*/

typedef enum { OK, LOST, CORRUPT } Outcome;

char msgs[MSG_COUNT][MAX_MSG_LEN] = {"HELLO", "OpenCamp", "NCCL", "2026"};
int msg_idx = 0;     /* 当前发送第几条消息 */
int seq_snd = 0;     /* 发送方的当前序号 (0 或 1) */
int seq_exp = 0;     /* 接收方期望的序号 (0 或 1) */
int send_count = 0;  /* 总发送次数（含重传）*/
int retrans_cnt = 0; /* 重传次数 */
int deliv_cnt = 0;   /* 成功交付次数 */

/* ─── 信道模拟：返回包在信道中的命运 ─── */
static Outcome channel(void) {
    int r = rand() % 100;
    if (r < LOSS_RATE) return LOST;
    if (r < LOSS_RATE + CORRUPT_RATE) return CORRUPT;
    return OK;
}

/* ─── ACK 是否在返回途中丢失？ ─── */
static int ack_lost(void) { return (rand() % 100) < ACK_LOSS_RATE; }

/* ─── 日志输出 ─── */
static void log_event(int num, const char *tag, int seq, const char *data) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 输出格式："[NN] TAG   seq=N \"DATA\"\n"
     * 示例：    "[03] SEND  seq=0 \"HELLO\"\n"
     *           "[03] LOST  seq=0\n"
     * TAG 占 5 列左对齐，data 为空时不输出双引号部分 */
}

/* ─── 发送方：发送当前消息 ─── */
static void sender_send(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 调用 log_event 记录 SEND 事件，seq 用 seq_snd，data 用 msgs[msg_idx]
     * send_count 加 1 */
}

/* ─── 发送方：处理 ACK (arrived=1) 或超时 (arrived=0) ─── */
static int sender_ack(int ack_seq, int arrived) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 若 arrived==0 (超时):
     *   → log_event TOUT, retrans_cnt++ → 返回 0（未完成当前消息）
     *
     * 若 arrived==1 (收到 ACK):
     *   → log_event ACK
     *   → 若 ack_seq == seq_snd（序号正确）:
     *       msg_idx++ → seq_snd = 1 - seq_snd（翻转序号）
     *       → 返回 1（当前消息完成）
     *   → 否则返回 0（序号不对，忽略此 ACK）
     *
     * 注意：序号 0/1 交替保证了发送方不会把旧 ACK 当成新 ACK */
}

/* ─── 接收方：处理收到的数据包 ─── */
static void receiver_recv(int pkt_seq, const char *data, int ok, int *send_ack, int *ack_seq) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 初始化 *send_ack = 0
     *
     * 若 ok==0 (包损坏):
     *   → log_event CORR, retrans_cnt++ → return（不发 ACK）
     *
     * 若 pkt_seq == seq_exp (序号匹配，是新包):
     *   → log_event RECV, deliv_cnt++ → seq_exp = 1 - seq_exp
     *   → *send_ack = 1, *ack_seq = pkt_seq
     *
     * 若 pkt_seq != seq_exp (序号不匹配，是重传的旧包):
     *   → log_event DUPL → *send_ack = 1, *ack_seq = pkt_seq
     *   → (重发 ACK 给发送方，但不重复交付)
     *
     * 关键：接收方只接受序号正确的包，用序号区分新包和重传包 */
}

/* ─── 初始化 ─── */
static void init(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* srand(42)
     * 将 msg_idx, seq_snd, seq_exp, send_count, retrans_cnt, deliv_cnt 置 0 */
}

/* ─── 主循环：停等协议 ─── */
int main(void) {
    init();

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 打印头部信息：
     * === RDT Stop-and-Wait ===
     * Msgs: HELLO OpenCamp NCCL 2026
     * Loss=25% Corrupt=10% ACKloss=10%
     * (空一行) */

    /* ═══ 停等协议主循环 ═══
     * while (msg_idx < MSG_COUNT && send_count < MAX_SENDS):
     *
     *   ① sender_send()  — 发送方发出当前包
     *
     *   ② oc = channel() — 模拟信道
     *
     *   ③ if (oc == LOST):
     *        → log_event LOST, retrans_cnt++, continue (重发)
     *
     *   ④ receiver_recv(seq_snd, msgs[msg_idx], oc==OK, &send_ack, &ack_seq)
     *      — 接收方处理到达的包
     *
     *   ⑤ if (send_ack):
     *        ack_arrived = !ack_lost()  — 模拟 ACK 回程
     *        if (!ack_arrived):
     *            → log_event ACK-L, retrans_cnt++, continue
     *        sender_ack(ack_seq, 1)     — 发送方收到 ACK
     *
     * ═══ 循环结束，输出统计 ═══
     * (空一行) === Stats ===
     * Delivered: D/N
     * Sends: S  Retrans: R */

    /* 统计输出提示：
     * printf("\n=== Stats ===\n");
     * printf("Delivered: %d/%d\n", deliv_cnt, MSG_COUNT);
     * printf("Sends: %d  Retrans: %d\n", send_count, retrans_cnt); */
    return 0;
}
