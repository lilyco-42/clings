/* 67_cooley-tukey-fft/fft.c — Cooley-Tukey FFT (N=8)
 *
 * Task: Implement the Cooley-Tukey Fast Fourier Transform for N=8.
 *
 * The Cooley-Tukey algorithm computes the Discrete Fourier Transform (DFT)
 * in O(N log N) time by recursively dividing the input into even-indexed
 * and odd-indexed samples. For N=8, there are 3 stages (log2(8)=3) of
 * butterfly operations.
 *
 * Input (fixed):  x[8] = {1, 2, 3, 4, 4, 3, 2, 1}
 *
 * Steps:
 *   1. Bit-reversal permutation: reorder input so adjacent pairs form butterflies
 *   2. Stage 1: Ns=2, 4 butterflies, twiddle factor W8^0
 *   3. Stage 2: Ns=4, 4 butterflies, twiddle factors W8^0, W8^2
 *   4. Stage 3: Ns=8, 4 butterflies, twiddle factors W8^0, W8^1, W8^2, W8^3
 *
 * Twiddle factors: W_N^k = e^(-2πi*k/N) = cos(2πk/N) - i*sin(2πk/N)
 *
 * Key concepts: FFT, butterfly, bit-reversal, twiddle factor, complex numbers,
 *                divide-and-conquer, spectral analysis
 *
 * Verification: make test compares against expected_output.txt
 */
#include <math.h>
#include <stdio.h>

#include "complex.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define N 8

/* Input time-domain samples (provided, fixed) */
static double input[N] = {1.0, 2.0, 3.0, 4.0, 4.0, 3.0, 2.0, 1.0};

/* Twiddle factor table: W8^0, W8^1, W8^2, W8^3 (provided)
 * W8^k = cos(2πk/8) - i*sin(2πk/8) */
static Complex W[4] = {
    {1.0, 0.0},                                /* W8^0 */
    {0.7071067811865476, -0.7071067811865476}, /* W8^1 =  √2/2 - i√2/2 */
    {0.0, -1.0},                               /* W8^2 = -i */
    {-0.7071067811865476, -0.7071067811865476} /* W8^3 = -√2/2 - i√2/2 */
};

/* ─── TODO 1: bit_reverse ───
 * Perform bit-reversal permutation on the complex array.
 * For N=8 (3 bits): swap elements so index i maps to bit-reversed index.
 * Only swap when i < j to avoid double-swapping.
 *
 * Algorithm: for i from 1 to n-1:
 *   find the next bit-reversed index j (use bit-shifting pattern)
 *   if i < j, swap a[i] and a[j]
 *
 * Bit-reversal for N=8:
 *   0(000)↔0(000), 1(001)↔4(100), 2(010)↔2(010), 3(011)↔6(110),
 *   4(100)↔1(001), 5(101)↔5(101), 6(110)↔3(011), 7(111)↔7(111) */
static void bit_reverse(Complex a[], int n) {
#error TODO 1: Implement bit-reversal permutation — iterate i from 1 to n-1, compute bit-reversed j using the bit-shifting algorithm, swap a[i] and a[j] when i < j.
}

/* ─── TODO 2: butterfly ───
 * Perform one stage of butterfly operations.
 *
 * Parameters:
 *   a[]    — complex array (already bit-reversed)
 *   n      — total size (8)
 *   stage  — which stage (1, 2, or 3)
 *
 * For stage s:
 *   group_size = 2^s   (use 1 << stage)
 *   half = group_size / 2
 *   step = n / group_size   (twiddle index step)
 *
 * For each group g (0, group_size, 2*group_size, ...):
 *   For each k in [0, half):
 *     even_idx = g + k
 *     odd_idx  = g + k + half
 *     twiddle_idx = k * step
 *
 *     T = W[twiddle_idx] * a[odd_idx]   (complex multiplication)
 *     new_even = a[even_idx] + T
 *     new_odd  = a[even_idx] - T
 *
 * Complex multiplication: (a+bi)*(c+di) = (ac-bd) + (ad+bc)i */
static void butterfly(Complex a[], int n, int stage) {
#error TODO 2: Implement one butterfly stage — compute group_size, half, step; nested loops over groups and k; complex multiply T = W * odd; update even = even+T, odd = even-T.
}

/* ─── TODO 3: fft ───
 * Compute the full FFT: bit-reverse, then 3 stages of butterflies.
 * Print intermediate results after bit-reversal and after each stage.
 *
 * Print format (exact!):
 *   printf("Bit-reversed order:\n");
 *   printf("  [%d]: %10.6f%+10.6fi\n", i, a[i].real, a[i].imag);
 *   printf("Stage %d (group_size=%d):\n", stage, 1 << stage);
 *   (same format for each element)
 *
 * Use %+10.6f for imag to get the sign automatically. */
static void fft(Complex a[], int n) {
#error TODO 3: Implement full FFT — call bit_reverse, print bit-reversed order, then loop stage=1..3 calling butterfly and printing each stage result.
}

/* ─── TODO 4: print_complex ───
 * Print a single complex number in the format "real+imagi"
 * Example: printf("%.6f%+.6fi", c.real, c.imag); */
static void print_complex(Complex c) {
#error TODO 4: Implement print_complex — use printf with %.6f for real and %+.6f for imag.
}

/* ─── TODO 5: main ───
 * 1. Build Complex array a[N] from input[N] (imag = 0.0)
 * 2. Print header: "=== Cooley-Tukey FFT (N=8) ===\n"
 * 3. Print input line and blank line
 * 4. Call fft(a, N)
 * 5. Print blank line and final frequency-domain result
 *    Format: "  X[%d] = ", then print_complex, then "\n"
 * 6. Return 0 */
int main(void) {
#error TODO 5: Implement main — build complex array from input, print header and input, call fft(), print final result.
}
