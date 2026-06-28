/* 66_turing-machine-sim/turing_machine.c — Turing Machine Simulator for a^n b^n
 *
 * Task: Implement a Turing machine that recognizes the language L = {a^n b^n | n >= 1}
 *
 * Turing Machine Definition:
 *   States:        q0, q1, q2, q3 (accept), q_reject
 *   Input alphabet: {a, b}
 *   Tape alphabet:  {a, b, X, Y, _}  (_ = blank)
 *   Start state:    q0
 *   Accept state:   q3
 *   Reject state:   q_reject
 *
 * Transition Rules:
 *   q0: read 'a' -> write 'X', move R, go to q1
 *   q0: read 'Y' -> write 'Y', move R, go to q3
 *   q1: read 'a' -> write 'a', move R, go to q1
 *   q1: read 'Y' -> write 'Y', move R, go to q1
 *   q1: read 'b' -> write 'Y', move L, go to q2
 *   q2: read 'a' -> write 'a', move L, go to q2
 *   q2: read 'Y' -> write 'Y', move L, go to q2
 *   q2: read 'X' -> write 'X', move R, go to q0
 *   q3: read 'Y' -> write 'Y', move R, go to q3
 *   q3: read '_' -> write '_', move R, accept
 *
 * Verification: make test compares against expected_output.txt
 */
#include <stdio.h>
#include <string.h>

#define TAPE_SIZE 64
#define BLANK '_'

/* Direction of head movement */
#define LEFT -1
#define RIGHT 1
#define HALT 0

/* State definitions */
#define Q0 0
#define Q1 1
#define Q2 2
#define Q3 3 /* accept */
#define Q_REJECT 4

/* Transition table entry */
typedef struct {
    int next_state;
    char write_symbol;
    int move; /* LEFT, RIGHT, or HALT */
} Transition;

/* Transition table: transition[state][symbol_index]
 * symbol_index: 0='a', 1='b', 2='X', 3='Y', 4='_' (blank)
 *
 * Special sentinel: next_state = -1 means "no transition → reject"
 */
static const Transition TRANSITION[5][5] = {
    /* q0 */ {
        /* a */ {Q1, 'X', RIGHT}, /* q0, a -> X, R, q1 */
        /* b */ {-1, 'b', HALT},  /* no rule -> reject */
        /* X */ {-1, 'X', HALT},  /* no rule -> reject */
        /* Y */ {Q3, 'Y', RIGHT}, /* q0, Y -> Y, R, q3 */
        /* _ */ {-1, '_', HALT},  /* no rule -> reject (n=0) */
    },
    /* q1 */
    {
        /* a */ {Q1, 'a', RIGHT}, /* q1, a -> a, R, q1 */
        /* b */ {Q2, 'Y', LEFT},  /* q1, b -> Y, L, q2 */
        /* X */ {-1, 'X', HALT},  /* no rule -> reject */
        /* Y */ {Q1, 'Y', RIGHT}, /* q1, Y -> Y, R, q1 */
        /* _ */ {-1, '_', HALT},  /* no rule -> reject */
    },
    /* q2 */
    {
        /* a */ {Q2, 'a', LEFT},  /* q2, a -> a, L, q2 */
        /* b */ {-1, 'b', HALT},  /* no rule -> reject */
        /* X */ {Q0, 'X', RIGHT}, /* q2, X -> X, R, q0 */
        /* Y */ {Q2, 'Y', LEFT},  /* q2, Y -> Y, L, q2 */
        /* _ */ {-1, '_', HALT},  /* no rule -> reject */
    },
    /* q3 */
    {
        /* a */ {-1, 'a', HALT},  /* no rule -> reject */
        /* b */ {-1, 'b', HALT},  /* no rule -> reject */
        /* X */ {-1, 'X', HALT},  /* no rule -> reject */
        /* Y */ {Q3, 'Y', RIGHT}, /* q3, Y -> Y, R, q3 */
        /* _ */ {Q3, '_', HALT},  /* q3, _ -> accept (halt) */
    },
    /* q_reject */
    {
        /* a */ {-1, 'a', HALT},
        /* b */ {-1, 'b', HALT},
        /* X */ {-1, 'X', HALT},
        /* Y */ {-1, 'Y', HALT},
        /* _ */ {-1, '_', HALT},
    },
};

static const char *STATE_NAMES[] = {"q0", "q1", "q2", "q3", "q_reject"};

/* Map tape symbol to index for transition table lookup */
static int sym_to_idx(char c) {
    switch (c) {
        case 'a':
            return 0;
        case 'b':
            return 1;
        case 'X':
            return 2;
        case 'Y':
            return 3;
        case '_':
            return 4;
        default:
            return 4; /* treat unknown as blank */
    }
}

/* ─── init_tape ───
 * Copy the input string onto the tape, surrounded by blanks.
 * Returns the length of the input (head starts at position 0). */
static int init_tape(char tape[], int tape_size, const char *input) {
    int i;
    for (i = 0; i < tape_size; i++) {
        tape[i] = BLANK;
    }
    int len = (int)strlen(input);
    for (i = 0; i < len && i < tape_size; i++) {
        tape[i] = input[i];
    }
    return len;
}

/* ─── print_tape ───
 * Print the current tape content, head position, and current state.
 * Format: "Tape: [...], head=pos, state=qX" */
static void print_tape(const char tape[], int tape_size, int head, int state) {
    printf("Tape: [");
    /* Find the rightmost non-blank to limit printing */
    int rightmost = 0;
    for (int i = 0; i < tape_size; i++) {
        if (tape[i] != BLANK) rightmost = i;
    }
    /* Print from position 0 to rightmost+2 (show some blanks) */
    int print_end = rightmost + 2;
    if (print_end >= tape_size) print_end = tape_size - 1;
    for (int i = 0; i <= print_end; i++) {
        if (i > 0) printf(" ");
        printf("%c", tape[i]);
    }
    printf("], head=%d, state=%s\n", head, STATE_NAMES[state]);
}

/* ─── step ───
 * Execute one transition of the Turing machine.
 * Returns the new state, or -1 if no transition exists (reject).
 * Updates the tape and head position via pointers. */
static int step(char tape[], int tape_size, int *head, int state) {
    char symbol = tape[*head];
    int si = sym_to_idx(symbol);
    Transition t = TRANSITION[state][si];

    if (t.next_state == -1) {
        /* No valid transition → reject */
        return Q_REJECT;
    }

    /* Write new symbol */
    tape[*head] = t.write_symbol;

    /* Move head */
    *head += t.move;
    if (*head < 0) *head = 0;
    if (*head >= tape_size) *head = tape_size - 1;

    return t.next_state;
}

/* ─── run ───
 * Run the Turing machine on the given input string.
 * Returns 1 if accepted, 0 if rejected.
 * Prints each step's tape content, state, and head position. */
static int run(const char *input) {
    char tape[TAPE_SIZE];
    int head = 0;
    int state = Q0;
    int steps = 0;

    init_tape(tape, TAPE_SIZE, input);

    printf("Input: \"%s\"\n", input);
    printf("Initial: ");
    print_tape(tape, TAPE_SIZE, head, state);

    while (state != Q3 || tape[head] != BLANK) {
        /* Check for reject state */
        if (state == Q_REJECT) {
            printf("Step %d: ", steps);
            print_tape(tape, TAPE_SIZE, head, state);
            printf("Result: REJECT (no valid transition)\n\n");
            return 0;
        }

        /* For q3: if we just transitioned to q3 on '_', it's an accept */
        if (state == Q3 && tape[head] == BLANK) {
            steps++;
            printf("Step %d: ", steps);
            print_tape(tape, TAPE_SIZE, head, state);
            printf("Result: ACCEPT (halted in q3 on blank)\n\n");
            return 1;
        }

        /* Execute one step */
        int prev_state = state;
        state = step(tape, TAPE_SIZE, &head, state);

        steps++;
        printf("Step %d: ", steps);
        print_tape(tape, TAPE_SIZE, head, state);

        /* Check for reject after step */
        if (state == Q_REJECT) {
            printf("Result: REJECT (no valid transition from %s on '%c')\n\n", STATE_NAMES[prev_state], tape[head]);
            return 0;
        }

        /* If q3 reads blank, we accept */
        if (state == Q3 && tape[head] == BLANK) {
            printf("Result: ACCEPT (halted in q3 on blank)\n\n");
            return 1;
        }
    }

    /* If we exit the loop in q3, we accept */
    printf("Result: ACCEPT\n\n");
    return 1;
}

int main(void) {
    printf("=== Turing Machine Simulator for L = {a^n b^n | n >= 1} ===\n\n");

    /* Test 1: "aaabbb" — should be accepted (n=3) */
    run("aaabbb");

    /* Test 2: "aab" — should be rejected (n=2 a's, n=1 b) */
    run("aab");

    return 0;
}
