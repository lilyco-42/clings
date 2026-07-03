/* 70_qubit-gate-simulator/qubit.c — Single-Qubit Gate Simulator
 *
 * Task: Simulate a single qubit undergoing a sequence of quantum gates
 *       and finally measurement. The qubit starts in |0⟩ = [1, 0].
 *
 * Gate sequence: H → X → Z → H → Measure (100 trials)
 *
 * Gate matrices (provided as constants):
 *   H = 1/√2 [[ 1,  1],        X = [[0, 1],       Z = [[1,  0],
 *             [ 1, -1]]             [1, 0]]            [0, -1]]
 *
 * Key concepts: qubit, superposition, Bloch sphere, Pauli matrices,
 *                Hadamard gate, quantum measurement, wavefunction collapse
 *
 * You must implement:
 *   complex_mult, complex_add, apply_gate, print_state, measure, main
 *
 * 验证：构建用 make；判分/自测用 clings run / clings watch；查看期望输出用 clings tests 70
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

/* ─── TODO 1: complex_mult ───
 * Multiply two complex numbers.
 * Formula: (a + bi) * (c + di) = (a*c - b*d) + (a*d + b*c)i
 *
 * @param a  First complex number
 * @param b  Second complex number
 * @return   The product a * b
 */
static Complex complex_mult(Complex a, Complex b) {
#error TODO 1: Implement complex_mult — compute (a.real*b.real - a.imag*b.imag) for real part, (a.real*b.imag + a.imag*b.real) for imag part. Return a Complex with these two values.
}

/* ─── TODO 2: complex_add ───
 * Add two complex numbers.
 * Formula: (a + bi) + (c + di) = (a + c) + (b + d)i
 *
 * @param a  First complex number
 * @param b  Second complex number
 * @return   The sum a + b
 */
static Complex complex_add(Complex a, Complex b) {
#error TODO 2: Implement complex_add — add real parts together, add imag parts together. Return a Complex with these two sums.
}

/* ─── TODO 3: apply_gate ───
 * Apply a 2x2 gate matrix to the qubit state vector.
 *
 * Matrix-vector multiplication: |ψ'⟩ = G |ψ⟩
 *   new_state[0] = G[0][0]*state[0] + G[0][1]*state[1]
 *   new_state[1] = G[1][0]*state[0] + G[1][1]*state[1]
 *
 * Each multiplication is complex_mult, each addition is complex_add.
 * Use a temporary variable so you don't overwrite state[0] before
 * computing state[1].
 *
 * @param gate   The 2x2 gate matrix (provided: H, X, or Z)
 * @param state  The qubit state vector [α, β] — modified in place
 */
static void apply_gate(const Complex gate[2][2], Complex state[2]) {
#error TODO 3: Implement apply_gate — compute new0 = gate[0][0]*state[0] + gate[0][1]*state[1], new1 = gate[1][0]*state[0] + gate[1][1]*state[1]. Use complex_mult and complex_add. Store results back into state[0] and state[1].
}

/* ─── TODO 4: print_state ───
 * Print the qubit state in Dirac notation.
 *
 * Format exactly:
 *   "label\n"
 *   "( %+f %+fi )|0⟩ + ( %+f %+fi )|1⟩\n"
 *   "\n"
 *
 * The %+f format always prints a sign (+ or -) before the number.
 * Example output line:
 *   ( +0.707107 +0.000000i )|0⟩ + ( +0.707107 +0.000000i )|1⟩
 *
 * @param label  Descriptive label for this step (e.g., "Initial state |0⟩:")
 * @param state  The qubit state vector [α, β]
 */
static void print_state(const char *label, const Complex state[2]) {
#error TODO 4: Implement print_state — printf label with newline, then printf the state in Dirac notation format with 4 Complex fields: state[0].real, state[0].imag, state[1].real, state[1].imag. End with an extra blank line.
}

/* ─── TODO 5: measure ───
 * Simulate a single projective measurement of the qubit.
 *
 * The qubit collapses to |0⟩ with probability |α|² = α.real² + α.imag²,
 * and to |1⟩ with probability |β|² = 1 - |α|².
 *
 * Use the provided PRNG (Linear Congruential Generator) to get a
 * pseudo-random number in [0, 1):
 *   *seed = (1103515245u * (*seed) + 12345u) & 0x7FFFFFFFu;
 *   double r = (double)(*seed) / (double)0x80000000u;
 *
 * Then: if r < prob0, return 0 (measured |0⟩); else return 1 (measured |1⟩).
 *
 * @param state  The qubit state vector [α, β]
 * @param seed   Pointer to PRNG seed (updated on each call)
 * @return       0 if measured |0⟩, 1 if measured |1⟩
 */
static int measure(const Complex state[2], unsigned int *seed) {
#error TODO 5: Implement measure — compute prob0 = state[0].real² + state[0].imag², advance LCG seed, generate r in [0,1), return 0 if r < prob0 else 1.
}

/* ─── TODO 6: main ───
 * Main simulation flow:
 *
 * 1. Initialize qubit state to |0⟩ = [1+0i, 0+0i]
 * 2. Print initial state with print_state("Initial state |0⟩:", state)
 * 3. Apply H gate → print_state("After H gate (superposition):", state)
 * 4. Apply X gate → print_state("After X gate (Pauli-X/NOT):", state)
 * 5. Apply Z gate → print_state("After Z gate (Pauli-Z):", state)
 * 6. Apply H gate → print_state("After second H gate:", state)
 * 7. Measure 100 times:
 *    - Declare int counts[2] = {0, 0} and unsigned int seed = 42u
 *    - Loop 100 times: int outcome = measure(state, &seed); counts[outcome]++
 * 8. Print measurement results:
 *    printf("Measurement results (%d trials):\n", 100);
 *    printf("|0⟩: %d (%.2f%%)\n", counts[0], counts[0] * 100.0 / 100);
 *    printf("|1⟩: %d (%.2f%%)\n", counts[1], counts[1] * 100.0 / 100);
 *
 * Return 0.
 */
int main(void) {
#error TODO 6: Implement main — initialize |0⟩ state, apply H→X→Z→H gates printing after each, measure 100 times, print results. Return 0.
}
