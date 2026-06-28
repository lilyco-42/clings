/* 50_rdt-stop-and-wait — solution */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_COUNT 4
#define MAX_MSG_LEN 16
#define LOSS_RATE 25
#define CORRUPT_RATE 10
#define ACK_LOSS_RATE 10
#define MAX_SENDS 30

typedef enum { OK, LOST, CORRUPT } Outcome;

char msgs[MSG_COUNT][MAX_MSG_LEN] = {"HELLO", "OpenCamp", "NCCL", "2026"};
int msg_idx = 0;
int seq_snd = 0;
int seq_exp = 0;
int send_count = 0;
int retrans_cnt = 0;
int deliv_cnt = 0;

static Outcome channel(void) {
    int r = rand() % 100;
    if (r < LOSS_RATE) return LOST;
    if (r < LOSS_RATE + CORRUPT_RATE) return CORRUPT;
    return OK;
}

static int ack_lost(void) { return (rand() % 100) < ACK_LOSS_RATE; }

static void log_event(int num, const char *tag, int seq, const char *data) {
    printf("[%02d] %-5s seq=%d", num, tag, seq);
    if (data[0]) printf(" \"%s\"", data);
    printf("\n");
}

static void sender_send(void) {
    log_event(send_count, "SEND", seq_snd, msgs[msg_idx]);
    send_count++;
}

static int sender_ack(int ack_seq, int arrived) {
    if (!arrived) {
        log_event(send_count, "TOUT", seq_snd, "");
        retrans_cnt++;
        return 0;
    }
    log_event(send_count, "ACK", ack_seq, "");
    if (ack_seq == seq_snd) {
        msg_idx++;
        seq_snd = 1 - seq_snd;
        return 1;
    }
    return 0;
}

static void receiver_recv(int pkt_seq, const char *data, int ok, int *send_ack, int *ack_seq) {
    *send_ack = 0;
    if (!ok) {
        log_event(send_count, "CORR", pkt_seq, "");
        retrans_cnt++;
        return;
    }
    if (pkt_seq == seq_exp) {
        log_event(send_count, "RECV", pkt_seq, data);
        deliv_cnt++;
        seq_exp = 1 - seq_exp;
        *send_ack = 1;
        *ack_seq = pkt_seq;
    } else {
        log_event(send_count, "DUPL", pkt_seq, "");
        *send_ack = 1;
        *ack_seq = pkt_seq;
    }
}

static void init(void) {
    srand(42);
    msg_idx = seq_snd = seq_exp = 0;
    send_count = retrans_cnt = deliv_cnt = 0;
}

int main(void) {
    init();
    printf("=== RDT Stop-and-Wait ===\n");
    printf("Msgs: HELLO OpenCamp NCCL 2026\n");
    printf("Loss=%d%% Corrupt=%d%% ACKloss=%d%%\n\n", LOSS_RATE, CORRUPT_RATE, ACK_LOSS_RATE);

    while (msg_idx < MSG_COUNT && send_count < MAX_SENDS) {
        sender_send();
        Outcome oc = channel();
        if (oc == LOST) {
            log_event(send_count, "LOST", seq_snd, "");
            retrans_cnt++;
            continue;
        }
        int send_ack = 0, ack_seq = 0;
        receiver_recv(seq_snd, msgs[msg_idx], oc == OK, &send_ack, &ack_seq);
        if (send_ack) {
            int ack_arrived = !ack_lost();
            if (!ack_arrived) {
                log_event(send_count, "ACK-L", ack_seq, "");
                retrans_cnt++;
                continue;
            }
            sender_ack(ack_seq, 1);
        }
    }

    printf("\n=== Stats ===\n");
    printf("Delivered: %d/%d\n", deliv_cnt, MSG_COUNT);
    printf("Sends: %d  Retrans: %d\n", send_count, retrans_cnt);
    return 0;
}
