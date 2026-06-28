/* 70_qubit-gate-simulator/qubit.c — Single-Qubit Gate Simulator
 *
 * Task: Simulate a single qubit undergoing a sequence of quantum gates
 *       and finally measurement. The qubit starts in |0⟩ = [1, 0].
 *
 * Gate sequence: H → X → Z → H → Measure
 *
 * Gate matrices (provided as constants):
 *   H = 1/√2 [[ 1,  1],        X = [[0, 1],       Z = [[1,  0],
 *             [ 1, -1]]             [1, 0]]            [0, -1]]
 *
 * Key concepts: qubit, superposition, Bloch sphere, Pauli matrices,
 *                Hadamard gate, quantum measurement, wavefunction collapse
 *
 * Verification: make test compares against expected_output.txt
 */
#include <math.h>
#include <stdio.h>

#include "complex.h"

/* ─── Gate matrix constants (provided) ───
 *
 * Hadamard gate H = 1/√2 [[1, 1], [1, -1]]
 *   - Creates superposition: |0⟩ → (|0⟩+|1⟩)/√2
 *   - Its own inverse: H·H = I
 *
 * Pauli-X gate (NOT) = [[0, 1], [1, 0]]
 *   - Flips |0⟩ ↔ |1⟩ (like classical NOT)
 *
 * Pauli-Z gate = [[1, 0], [0, -1]]
 *   - Flips the phase of |1⟩: Z|ψ⟩ = α|0⟩ - β|1⟩
 */
#define SQRT2_2 0.7071067811865476 /* 1/√2 */

static const Complex H[2][2] = {
    {{SQRT2_2, 0}, {SQRT2_2, 0}},
    {{SQRT2_2, 0}, {-SQRT2_2, 0}},
};

static const Complex X[2][2] = {
    {{0, 0}, {1, 0}},
    {{1, 0}, {0, 0}},
};

static const Complex Z[2][2] = {
    {{1, 0}, {0, 0}},
    {{0, 0}, {-1, 0}},
};

/* ─── complex_mult ───
 * Multiply two complex numbers: (a+bi)*(c+di) = (ac-bd) + (ad+bc)i */
static Complex complex_mult(Complex a, Complex b) {
    Complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

/* ─── complex_add ───
 * Add two complex numbers: (a+bi)+(c+di) = (a+c)+(b+d)i */
static Complex complex_add(Complex a, Complex b) {
    Complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

/* ─── apply_gate ───
 * Apply a 2x2 gate matrix G to the qubit state vector |ψ⟩.
 * New state: |ψ'⟩ = G |ψ⟩
 *   |ψ'⟩[0] = G[0][0]*|ψ⟩[0] + G[0][1]*|ψ⟩[1]
 *   |ψ'⟩[1] = G[1][0]*|ψ⟩[0] + G[1][1]*|ψ⟩[1]
 *
 * The result is written back into state[]. */
static void apply_gate(const Complex gate[2][2], Complex state[2]) {
    Complex new0 = complex_add(complex_mult(gate[0][0], state[0]), complex_mult(gate[0][1], state[1]));
    Complex new1 = complex_add(complex_mult(gate[1][0], state[0]), complex_mult(gate[1][1], state[1]));
    state[0] = new0;
    state[1] = new1;
}

/* ─── print_state ───
 * Print the qubit state vector in Dirac notation form:
 *   |ψ⟩ = α|0⟩ + β|1⟩
 * Format: "( 0.707107 + 0.000000i )|0⟩ + ( 0.707107 + 0.000000i )|1⟩\n"
 *
 * real and imag are printed with %+f to always show sign. */
static void print_state(const char *label, const Complex state[2]) {
    printf("%s\n", label);
    printf("( %+f %+fi )|0⟩ + ( %+f %+fi )|1⟩\n", state[0].real, state[0].imag, state[1].real, state[1].imag);
    printf("\n");
}

/* ─── measure ───
 * Perform a single measurement of the qubit.
 * The qubit collapses to |0⟩ with probability |α|²,
 * and to |1⟩ with probability |β|².
 *
 * Implementation: generate a random double r in [0, 1).
 * If r < |α|² (i.e., r < α.real² + α.imag²), return 0.
 * Otherwise, return 1.
 *
 * NOTE: For deterministic testing, we use a simple PRNG seeded
 * with a fixed value. In a real quantum system, measurement is
 * inherently probabilistic. */
static int measure(const Complex state[2], unsigned int *seed) {
    double prob0 = state[0].real * state[0].real + state[0].imag * state[0].imag;
    /* Simple LCG: X_{n+1} = (1103515245 * X_n + 12345) mod 2^31 */
    *seed = (1103515245u * (*seed) + 12345u) & 0x7FFFFFFFu;
    double r = (double)(*seed) / (double)0x80000000u;
    return (r < prob0) ? 0 : 1;
}

int main(void) {
    Complex state[2];
    unsigned int seed = 42u; /* fixed seed for reproducibility */
    int counts[2] = {0, 0};
    const int trials = 100;

    /* ─── Step 1: Initialize qubit to |0⟩ ─── */
    state[0].real = 1.0;
    state[0].imag = 0.0;
    state[1].real = 0.0;
    state[1].imag = 0.0;
    print_state("Initial state |0⟩:", state);

    /* ─── Step 2: Apply Hadamard gate → superposition ─── */
    apply_gate(H, state);
    print_state("After H gate (superposition):", state);

    /* ─── Step 3: Apply Pauli-X gate (NOT) ─── */
    apply_gate(X, state);
    print_state("After X gate (Pauli-X/NOT):", state);

    /* ─── Step 4: Apply Pauli-Z gate (phase flip) ─── */
    apply_gate(Z, state);
    print_state("After Z gate (Pauli-Z):", state);

    /* ─── Step 5: Apply Hadamard gate → back near |0⟩ ─── */
    apply_gate(H, state);
    print_state("After second H gate:", state);

    /* ─── Step 6: Measure 100 times and count outcomes ─── */
    for (int i = 0; i < trials; i++) {
        int outcome = measure(state, &seed);
        counts[outcome]++;
    }

    printf("Measurement results (%d trials):\n", trials);
    printf("|0⟩: %d (%.2f%%)\n", counts[0], (double)counts[0] * 100.0 / trials);
    printf("|1⟩: %d (%.2f%%)\n", counts[1], (double)counts[1] * 100.0 / trials);

    return 0;
}
