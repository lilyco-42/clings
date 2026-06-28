/* 49_dining-philosophers-sync — solution
 *
 * 状态机模拟哲学家就餐，免 pthread。
 * 解决死锁：P0-P3 左手优先，P4 右手优先（非对称策略）。
 */
#include <stdio.h>
#include <stdlib.h>

#define N 5
#define TARGET_EAT 3
#define MAX_ROUNDS 200
#define THINK_TIME_MIN 2
#define THINK_TIME_MAX 5
#define EAT_TIME 1

typedef enum { THINKING, HUNGRY, EATING } State;

State state[N];
int chopstick[N];
int eat_count[N];
int timer[N];

static void init(void) {
    srand(42);
    for (int i = 0; i < N; i++) {
        state[i] = THINKING;
        chopstick[i] = -1;
        eat_count[i] = 0;
        timer[i] = rand() % (THINK_TIME_MAX - THINK_TIME_MIN + 1) + THINK_TIME_MIN;
    }
}

static int can_eat(int id) {
    int left = id;
    int right = (id + 1) % N;
    if (id != 4)
        return chopstick[left] == -1 && chopstick[right] == -1;
    else
        return chopstick[right] == -1 && chopstick[left] == -1;
}

static void pickup(int id) {
    int left = id;
    int right = (id + 1) % N;
    if (id != 4) {
        chopstick[left] = id;
        chopstick[right] = id;
    } else {
        chopstick[right] = id;
        chopstick[left] = id;
    }
}

static void putdown(int id) {
    int left = id;
    int right = (id + 1) % N;
    chopstick[left] = -1;
    chopstick[right] = -1;
}

static int all_done(void) {
    for (int i = 0; i < N; i++)
        if (eat_count[i] < TARGET_EAT) return 0;
    return 1;
}

static void print_state(int round) {
    printf("%02d |", round);
    for (int i = 0; i < N; i++) {
        char c = state[i] == THINKING ? 'T' : state[i] == HUNGRY ? 'H' : 'E';
        printf(" P%d:%c", i, c);
    }
    printf("\n");
}

int main(void) {
    init();
    printf("=== Dining Philosophers ===\n");
    printf("N=5, asymmetric (P4 right-first), target=%d meals each\n", TARGET_EAT);
    printf("T=Thinking H=Hungry E=Eating\n\n");

    int round = 0;
    while (round < MAX_ROUNDS) {
        print_state(round);

        /* Phase 1: EATING → finish */
        for (int i = 0; i < N; i++) {
            if (state[i] == EATING) {
                timer[i]--;
                if (timer[i] == 0) {
                    putdown(i);
                    eat_count[i]++;
                    state[i] = THINKING;
                    timer[i] = rand() % (THINK_TIME_MAX - THINK_TIME_MIN + 1) + THINK_TIME_MIN;
                }
            }
        }

        if (all_done()) break;

        /* Phase 2: HUNGRY → try to eat */
        for (int i = 0; i < N; i++) {
            if (state[i] == HUNGRY && can_eat(i)) {
                pickup(i);
                state[i] = EATING;
                timer[i] = EAT_TIME;
            }
        }

        /* Phase 3: THINKING → HUNGRY */
        for (int i = 0; i < N; i++) {
            if (state[i] == THINKING) {
                timer[i]--;
                if (timer[i] == 0) state[i] = HUNGRY;
            }
        }

        round++;
    }

    printf("\n=== Final Stats ===\n");
    for (int i = 0; i < N; i++) printf("P%d ate %d time(s)\n", i, eat_count[i]);
    printf("Total rounds: %d\n", round);
    return 0;
}
