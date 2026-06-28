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

/* Input time-domain samples (fixed) */
static double input[N] = {1.0, 2.0, 3.0, 4.0, 4.0, 3.0, 2.0, 1.0};

/* Twiddle factor table: W8^0, W8^1, W8^2, W8^3
 * W8^k = cos(2πk/8) - i*sin(2πk/8) */
static Complex W[4] = {
    {1.0, 0.0},                                /* W8^0 */
    {0.7071067811865476, -0.7071067811865476}, /* W8^1 =  √2/2 - i√2/2 */
    {0.0, -1.0},                               /* W8^2 = -i */
    {-0.7071067811865476, -0.7071067811865476} /* W8^3 = -√2/2 - i√2/2 */
};

/* ─── bit_reverse ───
 * Perform bit-reversal permutation on the complex array.
 * For N=8 (3 bits): swap elements so index i goes to bit-reversed index.
 * Only swap when i < j to avoid double-swapping. */
static void bit_reverse(Complex a[], int n) {
    int i, j;
    for (i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1) j ^= bit;
        j ^= bit;
        if (i < j) {
            Complex tmp = a[i];
            a[i] = a[j];
            a[j] = tmp;
        }
    }
}

/* ─── butterfly ───
 * Perform one stage of butterfly operations.
 * a: complex array (already bit-reversed)
 * n: total size (8)
 * stage: which stage (1, 2, or 3)
 *
 * For stage s, group size = 2^s, half = 2^(s-1)
 * Twiddle step = N / group_size = 8 / 2^s */
static void butterfly(Complex a[], int n, int stage) {
    int group_size = 1 << stage; /* 2, 4, 8 */
    int half = group_size >> 1;  /* 1, 2, 4 */
    int step = n / group_size;   /* 4, 2, 1 */
    int k, g;

    for (g = 0; g < n; g += group_size) {
        for (k = 0; k < half; k++) {
            int even_idx = g + k;
            int odd_idx = g + k + half;
            int twiddle_idx = k * step;

            Complex even = a[even_idx];
            Complex odd = a[odd_idx];
            Complex w = W[twiddle_idx];

            /* T = W * odd */
            Complex T;
            T.real = w.real * odd.real - w.imag * odd.imag;
            T.imag = w.real * odd.imag + w.imag * odd.real;

            /* a[even_idx] = even + T */
            a[even_idx].real = even.real + T.real;
            a[even_idx].imag = even.imag + T.imag;

            /* a[odd_idx] = even - T */
            a[odd_idx].real = even.real - T.real;
            a[odd_idx].imag = even.imag - T.imag;
        }
    }
}

/* ─── fft ───
 * Compute the full FFT: bit-reverse, then 3 stages of butterflies.
 * Print intermediate results after each stage. */
static void fft(Complex a[], int n) {
    int stage;

    /* Bit-reversal permutation */
    bit_reverse(a, n);

    printf("Bit-reversed order:\n");
    printf("  [0]: %10.6f%+10.6fi\n", a[0].real, a[0].imag);
    printf("  [1]: %10.6f%+10.6fi\n", a[1].real, a[1].imag);
    printf("  [2]: %10.6f%+10.6fi\n", a[2].real, a[2].imag);
    printf("  [3]: %10.6f%+10.6fi\n", a[3].real, a[3].imag);
    printf("  [4]: %10.6f%+10.6fi\n", a[4].real, a[4].imag);
    printf("  [5]: %10.6f%+10.6fi\n", a[5].real, a[5].imag);
    printf("  [6]: %10.6f%+10.6fi\n", a[6].real, a[6].imag);
    printf("  [7]: %10.6f%+10.6fi\n", a[7].real, a[7].imag);

    /* 3 stages of butterfly */
    for (stage = 1; stage <= 3; stage++) {
        butterfly(a, n, stage);

        printf("Stage %d (group_size=%d):\n", stage, 1 << stage);
        printf("  [0]: %10.6f%+10.6fi\n", a[0].real, a[0].imag);
        printf("  [1]: %10.6f%+10.6fi\n", a[1].real, a[1].imag);
        printf("  [2]: %10.6f%+10.6fi\n", a[2].real, a[2].imag);
        printf("  [3]: %10.6f%+10.6fi\n", a[3].real, a[3].imag);
        printf("  [4]: %10.6f%+10.6fi\n", a[4].real, a[4].imag);
        printf("  [5]: %10.6f%+10.6fi\n", a[5].real, a[5].imag);
        printf("  [6]: %10.6f%+10.6fi\n", a[6].real, a[6].imag);
        printf("  [7]: %10.6f%+10.6fi\n", a[7].real, a[7].imag);
    }
}

/* ─── print_complex ───
 * Print a single complex number in the format "real+imagi" */
static void print_complex(Complex c) { printf("%.6f%+.6fi", c.real, c.imag); }

/* ─── main ───
 * Build the complex input array from real samples, run FFT, print final result. */
int main(void) {
    Complex a[N];
    int i;

    /* Build complex array from real input samples */
    for (i = 0; i < N; i++) {
        a[i].real = input[i];
        a[i].imag = 0.0;
    }

    printf("=== Cooley-Tukey FFT (N=8) ===\n");
    printf("\n");
    printf("Input (time domain):\n");
    printf("  x = [1, 2, 3, 4, 4, 3, 2, 1]\n");
    printf("\n");

    /* Run FFT with intermediate printing */
    fft(a, N);

    printf("\n");
    printf("Final frequency-domain result:\n");
    for (i = 0; i < N; i++) {
        printf("  X[%d] = ", i);
        print_complex(a[i]);
        printf("\n");
    }

    return 0;
}
