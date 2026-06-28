/* 63_lu-decomposition-solver — LU Decomposition Solver with Partial Pivoting (solution)
 *
 * Solves Ax = b for a fixed 3x3 system:
 *   A = {{2,1,1},{4,3,3},{8,7,9}},  b = {5,11,29}
 * Solution: x = {2, -2, 3}
 *
 * Algorithm:
 *   1. PA = LU decomposition with partial pivoting (row swaps)
 *   2. Forward substitution: Ly = Pb
 *   3. Backward substitution: Ux = y
 */

#include <math.h>
#include <stdio.h>

#define N 3

/* Print an N×N matrix with a label */
void print_matrix(const char *label, double m[N][N]) {
    printf("%s:\n", label);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%8.4f ", m[i][j]);
        }
        printf("\n");
    }
}

/* Print an N-vector with a label */
void print_vector(const char *label, double v[N]) {
    printf("%s: [", label);
    for (int i = 0; i < N; i++) {
        printf("%8.4f%s", v[i], i < N - 1 ? " " : "");
    }
    printf("]\n");
}

/* Partial pivoting on column k: find row with max |A[i][k]|, swap */
void pivot(double A[N][N], int p[N], int k) {
    int max_row = k;
    double max_val = fabs(A[k][k]);
    for (int i = k + 1; i < N; i++) {
        if (fabs(A[i][k]) > max_val) {
            max_val = fabs(A[i][k]);
            max_row = i;
        }
    }
    if (max_row != k) {
        /* Swap rows in A */
        for (int j = 0; j < N; j++) {
            double tmp = A[k][j];
            A[k][j] = A[max_row][j];
            A[max_row][j] = tmp;
        }
        /* Record permutation */
        int tmp = p[k];
        p[k] = p[max_row];
        p[max_row] = tmp;

        printf("  Pivot: swap row %d <-> row %d\n", k, max_row);
    }
}

/* LU decomposition with partial pivoting.
 * On output, A contains both L and U factors in-place:
 *   - Upper triangle (i <= j): U[i][j]
 *   - Strict lower triangle (i > j): L[i][j] (unit diagonal implied)
 * p[] records the permutation applied.
 * L and U are extracted separately for clarity. */
void lu_decomp(double A[N][N], double L[N][N], double U[N][N], int p[N]) {
    /* Initialize permutation to identity */
    for (int i = 0; i < N; i++) {
        p[i] = i;
    }

    for (int k = 0; k < N; k++) {
        /* Partial pivoting */
        pivot(A, p, k);

        /* Compute multipliers and update trailing submatrix */
        for (int i = k + 1; i < N; i++) {
            A[i][k] = A[i][k] / A[k][k]; /* multiplier l_ik */
            for (int j = k + 1; j < N; j++) {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
    }

    /* Extract L and U from the factored A */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (i > j) {
                L[i][j] = A[i][j];
                U[i][j] = 0.0;
            } else if (i == j) {
                L[i][j] = 1.0;
                U[i][j] = A[i][j];
            } else {
                L[i][j] = 0.0;
                U[i][j] = A[i][j];
            }
        }
    }
}

/* Forward substitution: solve Ly = Pb for y */
void forward_subst(double L[N][N], double b[N], int p[N], double y[N]) {
    /* Apply permutation to b: Pb */
    double Pb[N];
    for (int i = 0; i < N; i++) {
        Pb[i] = b[p[i]];
    }

    /* Solve Ly = Pb: y_i = Pb_i - sum_{j=0}^{i-1} L[i][j] * y_j */
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int j = 0; j < i; j++) {
            sum += L[i][j] * y[j];
        }
        y[i] = Pb[i] - sum;
    }
}

/* Backward substitution: solve Ux = y for x */
void back_subst(double U[N][N], double y[N], double x[N]) {
    /* Solve Ux = y: x_i = (y_i - sum_{j=i+1}^{N-1} U[i][j] * x_j) / U[i][i] */
    for (int i = N - 1; i >= 0; i--) {
        double sum = 0.0;
        for (int j = i + 1; j < N; j++) {
            sum += U[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / U[i][i];
    }
}

/* Solve the full system: PA=LU, Ly=Pb, Ux=y */
void solve(double A_orig[N][N], double b[N], double x[N]) {
    double A[N][N], L[N][N], U[N][N];
    int p[N];

    /* Copy A since LU decomposition modifies it in-place */
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) A[i][j] = A_orig[i][j];

    printf("=== LU Decomposition Solver with Partial Pivoting ===\n\n");

    /* Step 1: PA = LU */
    printf("Step 1: PA = LU decomposition\n");
    lu_decomp(A, L, U, p);

    printf("\nPermutation vector p: [");
    for (int i = 0; i < N; i++) printf("%d%s", p[i], i < N - 1 ? " " : "");
    printf("]\n\n");

    print_matrix("L matrix", L);
    printf("\n");
    print_matrix("U matrix", U);

    /* Step 2: Forward substitution Ly = Pb */
    printf("\nStep 2: Forward substitution Ly = Pb\n");
    double y[N];
    forward_subst(L, b, p, y);
    print_vector("y", y);

    /* Step 3: Backward substitution Ux = y */
    printf("\nStep 3: Backward substitution Ux = y\n");
    back_subst(U, y, x);
    print_vector("x (solution)", x);

    /* Verify: compute Ax and compare to b */
    printf("\n=== Verification: Ax ===\n");
    double Ax[N];
    for (int i = 0; i < N; i++) {
        Ax[i] = 0.0;
        for (int j = 0; j < N; j++) {
            Ax[i] += A_orig[i][j] * x[j];
        }
    }
    printf("Ax: [");
    for (int i = 0; i < N; i++) {
        printf("%8.4f%s", Ax[i], i < N - 1 ? " " : "");
    }
    printf("]\n");
    printf("b:  [");
    for (int i = 0; i < N; i++) {
        printf("%8.4f%s", b[i], i < N - 1 ? " " : "");
    }
    printf("]\n");
}

int main(void) {
    /* Fixed problem: A = {{2,1,1},{4,3,3},{8,7,9}}, b = {5,11,29} */
    double A[N][N] = {{2.0, 1.0, 1.0}, {4.0, 3.0, 3.0}, {8.0, 7.0, 9.0}};
    double b[N] = {5.0, 11.0, 29.0};
    double x[N];

    solve(A, b, x);

    return 0;
}
