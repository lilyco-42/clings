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
 *
 * Transition Rules (provided as TRANSITION table):
 *   q0: read 'a' -> write 'X', move R, go to q1
 *   q0: read 'Y' -> write 'Y', move R, go to q3
 *   q1: read 'a' -> write 'a', move R, go to q1
 *   q1: read 'Y' -> write 'Y', move R, go to q1
 *   q1: read 'b' -> write 'Y', move L, go to q2
 *   q2: read 'a' -> write 'a', move L, go to q2
 *   q2: read 'Y' -> write 'Y', move L, go to q2
 *   q2: read 'X' -> write 'X', move R, go to q0
 *   q3: read 'Y' -> write 'Y', move R, go to q3
 *   q3: read '_' -> write '_', move R, accept (halt)
 *
 * Key concepts: Turing machine, transition function, tape, head,
 *                Church-Turing thesis, halting problem
 *
 * Verification: build with make; judge via `clings run 66` or `clings watch`;
 *               view expected output via `clings tests 66`
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
 * next_state = -1 means "no transition -> reject"
 *
 * This table encodes the entire Turing machine program.
 * You do NOT need to modify it — just use it in step().
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

/* Map tape symbol to index for transition table lookup (provided) */
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

/* ─── TODO 1: init_tape ───
 * Initialize the tape array with the input string.
 * Fill ALL positions with BLANK first, then copy input chars.
 * The head starts at position 0.
 * No return value — just fill the tape[] array. */
static void init_tape(char tape[], int tape_size, const char *input) {
#error TODO 1: Initialize tape — fill entire tape with BLANK, then copy each char of input. Use strlen() to get input length.
}

/* ─── TODO 2: print_tape ───
 * Print the current tape content, head position, and current state.
 * Format: "Tape: [a b c ...], head=N, state=qX\n"
 * Print only up to rightmost non-blank + 2 extra blanks.
 * Use printf for each symbol separated by space. */
static void print_tape(const char tape[], int tape_size, int head, int state) {
#error TODO 2: Print tape — find rightmost non-blank index, then print from 0 to rightmost+2. Use printf for symbols with space separator, then print head and state.
}

/* ─── TODO 3: step ───
 * Execute ONE transition of the Turing machine.
 * Look up TRANSITION[state][sym_to_idx(tape[*head])].
 * If next_state == -1: return Q_REJECT (no valid transition).
 * Otherwise: write the new symbol to tape[*head], update *head
 *   by adding t.move, clamp head to [0, tape_size-1],
 *   and return the new state. */
static int step(char tape[], int tape_size, int *head, int state) {
#error TODO 3: Step — get symbol index via sym_to_idx(tape[*head]), look up TRANSITION[state][si], if next_state is -1 return Q_REJECT, else write symbol, move head (clamp), return next_state.
}

/* ─── TODO 4: run ───
 * Run the Turing machine on the given input string.
 * Algorithm:
 *   1. Call init_tape() to set up the tape
 *   2. Print "Input: \"...\"" and initial tape/head/state
 *   3. Loop until accept or reject:
 *      a. If state == Q_REJECT: print step, "Result: REJECT", return 0
 *      b. If state == Q3 AND tape[head] == BLANK: print "Result: ACCEPT", return 1
 *      c. Call step() to execute one transition
 *      d. Print step info after each transition
 *      e. If step returned Q_REJECT: print reject message, return 0
 *      f. If state == Q3 AND tape[head] == BLANK: print accept message, return 1
 *   Return 1 if accepted, 0 if rejected. */
static int run(const char *input) {
#error TODO 4: Run — init_tape, print initial, loop: check reject/accept, call step, print after each step, handle final result.
}

/* ─── TODO 5: main ───
 * Print the header line:
 *   "=== Turing Machine Simulator for L = {a^n b^n | n >= 1} ===\n"
 * Then call run("aaabbb") and run("aab").
 * Return 0. */
int main(void) {
#error TODO 5: Main — print header, call run("aaabbb"), call run("aab"), return 0.
}
