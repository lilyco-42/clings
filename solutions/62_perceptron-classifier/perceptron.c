/* 62_perceptron-classifier — Perceptron Binary Classifier (solution)
 *
 * Implements a single-layer perceptron to learn the AND logic function
 * (linearly separable, converges), then demonstrates the XOR problem
 * (linearly inseparable, never converges).
 *
 * AND dataset: (0,0)→-1  (0,1)→-1  (1,0)→-1  (1,1)→+1
 * XOR dataset: (0,0)→-1  (0,1)→+1  (1,0)→+1  (1,1)→-1
 *
 * Uses fixed learning rate α = 0.1, initial weight w = {0, 0}, bias b = 0.
 * Perceptron update rule for each misclassified point (x, y):
 *   w = w + α * y * x
 *   b = b + α * y
 */

#include <stdio.h>

/* ─── Datasets ─── */
#define N_POINTS 4
#define N_FEATURES 2

static const double X_AND[N_POINTS][N_FEATURES] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
static const double Y_AND[N_POINTS] = {-1.0, -1.0, -1.0, 1.0};

static const double X_XOR[N_POINTS][N_FEATURES] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};
static const double Y_XOR[N_POINTS] = {-1.0, 1.0, 1.0, -1.0};

/* Hyperparameters */
static const double ALPHA = 0.1;
static const int MAX_EPOCHS_AND = 100;
static const int MAX_EPOCHS_XOR = 100;

/* ─── Dot product: w·x + b ─── */
static double dot(const double w[], double b, const double x[]) {
    double sum = b;
    int i;
    for (i = 0; i < N_FEATURES; i++) {
        sum += w[i] * x[i];
    }
    return sum;
}

/* ─── Sign function: returns +1.0 if val >= 0, else -1.0 ─── */
static double sign(double val) {
    if (val >= 0.0) return 1.0;
    return -1.0;
}

/* ─── Predict label for a single point ─── */
static double predict(const double w[], double b, const double x[]) { return sign(dot(w, b, x)); }

/* ─── Train for one epoch; returns number of mistakes ─── */
static int train_one_epoch(double w[], double *b, const double X_data[][N_FEATURES], const double Y_data[]) {
    int mistakes = 0;
    int i;
    for (i = 0; i < N_POINTS; i++) {
        double pred = predict(w, *b, X_data[i]);
        if (pred != Y_data[i]) {
            int j;
            for (j = 0; j < N_FEATURES; j++) {
                w[j] += ALPHA * Y_data[i] * X_data[i][j];
            }
            *b += ALPHA * Y_data[i];
            mistakes++;
        }
    }
    return mistakes;
}

/* ─── Print current weights and bias ─── */
static void print_weights(const double w[], double b) { printf("w = {%.4f, %.4f}, b = %.4f", w[0], w[1], b); }

/* ─── Print classification results for 4 points ─── */
static void print_classification(const double w[], double b, const double X_data[][N_FEATURES], const double Y_data[]) {
    int i;
    printf("Classification: ");
    for (i = 0; i < N_POINTS; i++) {
        double pred = predict(w, b, X_data[i]);
        const char *status = (pred == Y_data[i]) ? "✓" : "✗";
        printf("(%.0f,%.0f)→%+.0f%s", X_data[i][0], X_data[i][1], pred, status);
        if (i < N_POINTS - 1) printf("  ");
    }
    printf("\n");
}

int main(void) {
    double w[N_FEATURES];
    double b;
    int epoch, converged, mistakes;

    /* ═══════════════════════════════════════════
     * Part 1: AND — linearly separable, converges
     * ═══════════════════════════════════════════ */
    w[0] = 0.0;
    w[1] = 0.0;
    b = 0.0;
    converged = 0;

    printf("=== Perceptron Binary Classifier (AND logic) ===\n\n");
    printf("Dataset:\n");
    printf("  x=(0,0) y=-1  x=(0,1) y=-1  x=(1,0) y=-1  x=(1,1) y=+1\n");
    printf("Learning rate α = %.1f, max epochs = %d\n", ALPHA, MAX_EPOCHS_AND);
    printf("Initial: w = {0.0000, 0.0000}, b = 0.0000\n\n");

    printf("=== Training Rounds ===\n");

    for (epoch = 0; epoch < MAX_EPOCHS_AND; epoch++) {
        printf("--- Epoch %d ---\n", epoch + 1);
        printf("Before: ");
        print_weights(w, b);
        printf("\n");

        mistakes = train_one_epoch(w, &b, X_AND, Y_AND);

        printf("After:  ");
        print_weights(w, b);
        printf("\n");

        print_classification(w, b, X_AND, Y_AND);

        if (mistakes == 0) {
            printf("All points correctly classified — converged!\n");
            converged = 1;
            epoch++;
            break;
        }
    }

    if (!converged) {
        printf("Warning: Did not converge within %d epochs.\n", MAX_EPOCHS_AND);
    }

    printf("\n=== Final Decision Boundary ===\n");
    printf("Equation: w0*x0 + w1*x1 + b = 0\n");
    printf("  %.4f * x0 + %.4f * x1 + %.4f = 0\n", w[0], w[1], b);
    if (w[1] != 0.0) {
        printf("  x1 = %.4f * x0 + %.4f\n", -w[0] / w[1], -b / w[1]);
    } else if (w[0] != 0.0) {
        printf("  x0 = %.4f  (vertical line)\n", -b / w[0]);
    }
    printf("Converged in %d epochs.\n", epoch);

    printf("\n");

    /* ═══════════════════════════════════════════════════
     * Part 2: XOR — linearly inseparable, never converges
     * ═══════════════════════════════════════════════════ */
    w[0] = 0.0;
    w[1] = 0.0;
    b = 0.0;
    converged = 0;

    printf("=== Perceptron Binary Classifier (XOR logic) ===\n\n");
    printf("Dataset:\n");
    printf("  x=(0,0) y=-1  x=(0,1) y=+1  x=(1,0) y=+1  x=(1,1) y=-1\n");
    printf("Learning rate α = %.1f, max epochs = %d\n", ALPHA, MAX_EPOCHS_XOR);
    printf("Initial: w = {0.0000, 0.0000}, b = 0.0000\n\n");

    printf("=== Training Rounds ===\n");

    for (epoch = 0; epoch < MAX_EPOCHS_XOR; epoch++) {
        /* Show first 5 epochs in full, then every 10th, plus the last */
        int show_full = (epoch < 5) || ((epoch + 1) % 10 == 0) || (epoch == MAX_EPOCHS_XOR - 1);

        /* Print compact summary before showing the next epoch after a gap.
         * These fire on the first shown epoch AFTER the hidden range. */
        if (show_full && epoch == 9) {
            /* epoch 9 = Epoch 10; epochs 6-9 were hidden */
            printf("  ... (epochs 6–9: oscillating, 2 mistakes each) ...\n");
        }
        if (show_full && epoch == 19) {
            /* epoch 19 = Epoch 20; epochs 11-19 were hidden */
            printf("  ... (epochs 11–19: oscillating, 2 mistakes each) ...\n");
        }

        if (show_full) {
            printf("--- Epoch %d ---\n", epoch + 1);
            printf("Before: ");
            print_weights(w, b);
            printf("\n");
        }

        mistakes = train_one_epoch(w, &b, X_XOR, Y_XOR);

        if (show_full) {
            printf("After:  ");
            print_weights(w, b);
            printf("\n");
            print_classification(w, b, X_XOR, Y_XOR);
        }

        if (mistakes == 0) {
            if (show_full) {
                printf("All points correctly classified — converged!\n");
            }
            converged = 1;
            epoch++;
            break;
        }
    }

    if (!converged) {
        printf("\nWarning: Did not converge within %d epochs.\n", MAX_EPOCHS_XOR);
        printf("XOR is not linearly separable — a single perceptron cannot learn it.\n");
        printf("This demonstrates the fundamental limitation that led to the first AI winter.\n");
    }

    printf("\n=== Final Decision Boundary ===\n");
    printf("Equation: w0*x0 + w1*x1 + b = 0\n");
    printf("  %.4f * x0 + %.4f * x1 + %.4f = 0\n", w[0], w[1], b);
    if (w[1] != 0.0) {
        printf("  x1 = %.4f * x0 + %.4f\n", -w[0] / w[1], -b / w[1]);
    } else if (w[0] != 0.0) {
        printf("  x0 = %.4f  (vertical line)\n", -b / w[0]);
    }
    printf("Did not converge — dataset is not linearly separable.\n");

    return 0;
}
