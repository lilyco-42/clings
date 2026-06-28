/* 68_sobel-edge-detection/sobel.c — Sobel Edge Detection on PGM Images
 *
 * Task: Implement Sobel edge detection on a fixed 8x8 grayscale PGM image.
 *
 * The input image contains vertical, horizontal, diagonal edges and uniform regions.
 * The program:
 *   1. Prints the original 8x8 image matrix
 *   2. Applies 3x3 Sobel convolution (X and Y kernels) to each interior pixel
 *   3. Computes gradient magnitude G = sqrt(Gx² + Gy²)
 *   4. Thresholds at 128: ≥128 → 255, <128 → 0 (binary edge map)
 *   5. Prints the gradient magnitude matrix and the binary edge image
 *
 * Key concepts: image convolution, Sobel operator, gradient magnitude,
 *                edge detection, thresholding / binarization
 *
 * Verification: make test compares against expected_output.txt
 */
#include <math.h>
#include <stdio.h>

#define ROWS 8
#define COLS 8
#define THRESHOLD 128

/* ─── Image data (hardcoded 8x8 grayscale PGM) ───
 *
 * This image contains multiple edge types:
 *   - Vertical edge:   cols 0-1 are dark (~50), cols 2-3 are bright (~200)
 *                      in rows 0-2, creating a vertical edge at col 1→2.
 *   - Horizontal edge: rows 0-2 are mixed dark/bright, rows 3-5 are all
 *                      bright (~200), creating a horizontal edge at row 2→3.
 *   - Diagonal edge:   row 5 has a dark pixel at col 0, creating diagonal
 *                      transitions. Row 6 is all dark (~50).
 *   - Uniform region:  rows 6-7 are all dark (~50), producing zero gradient.
 */
static const int IMAGE[ROWS][COLS] = {
    {50, 50, 200, 200, 50, 50, 50, 50},       {50, 50, 200, 200, 50, 50, 50, 50},
    {50, 50, 200, 200, 50, 50, 50, 50},       {200, 200, 200, 200, 200, 200, 200, 200},
    {200, 200, 200, 200, 200, 200, 200, 200}, {50, 200, 200, 200, 200, 200, 200, 200},
    {50, 50, 50, 50, 50, 50, 50, 50},         {50, 50, 50, 50, 50, 50, 50, 50},
};

/* ─── Sobel kernels (provided) ───
 * Gx: detects vertical edges (horizontal gradient)
 * Gy: detects horizontal edges (vertical gradient)
 *
 *   Gx = [[-1, 0, 1],       Gy = [[-1,-2,-1],
 *         [-2, 0, 2],             [ 0, 0, 0],
 *         [-1, 0, 1]]             [ 1, 2, 1]]
 */
static const int GX[3][3] = {
    {-1, 0, 1},
    {-2, 0, 2},
    {-1, 0, 1},
};

static const int GY[3][3] = {
    {-1, -2, -1},
    {0, 0, 0},
    {1, 2, 1},
};

/* ─── print_matrix ───
 * Print an integer matrix with the given title.
 * Each element is printed with a fixed width for alignment.
 * Rows are separated by newlines. */
static void print_matrix(const char *title, int mat[ROWS][COLS]) {
    printf("%s\n", title);
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (j > 0) printf(" ");
            printf("%3d", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/* ─── convolve ───
 * Apply a 3x3 convolution kernel to the image at pixel (row, col).
 * The kernel is applied to the 3x3 neighborhood centered at (row, col).
 * Returns the weighted sum of the neighborhood * kernel. */
static int convolve(int image[ROWS][COLS], const int kernel[3][3], int row, int col) {
    int sum = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            sum += image[row + i][col + j] * kernel[i + 1][j + 1];
        }
    }
    return sum;
}

/* ─── gradient_magnitude ───
 * Compute the gradient magnitude at pixel (row, col) using the Sobel operator.
 *   1. Convolve with GX to get Gx
 *   2. Convolve with GY to get Gy
 *   3. Return G = sqrt(Gx² + Gy²), rounded to the nearest integer. */
static int gradient_magnitude(int image[ROWS][COLS], int row, int col) {
    int gx = convolve(image, GX, row, col);
    int gy = convolve(image, GY, row, col);
    /* sqrt returns double; add 0.5 and cast to int for rounding */
    return (int)(sqrt((double)(gx * gx + gy * gy)) + 0.5);
}

/* ─── threshold ───
 * Apply binary thresholding.
 *   If value >= THRESHOLD → return 255 (edge / white)
 *   Otherwise           → return   0 (non-edge / black) */
static int threshold(int value) { return (value >= THRESHOLD) ? 255 : 0; }

int main(void) {
    int grad_mag[ROWS][COLS];
    int edge_binary[ROWS][COLS];

    /* Step 1: Print the original image */
    print_matrix("Original Image:", (int(*)[COLS])IMAGE);

    /* Step 2 & 3: Compute gradient magnitude for every pixel.
     * Border pixels (row 0, row ROWS-1, col 0, col COLS-1) have incomplete
     * 3x3 neighborhoods — set them to 0. */
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
                grad_mag[i][j] = 0;
            } else {
                grad_mag[i][j] = gradient_magnitude((int(*)[COLS])IMAGE, i, j);
            }
        }
    }

    /* Step 4: Threshold the gradient magnitude to get binary edge map */
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            edge_binary[i][j] = threshold(grad_mag[i][j]);
        }
    }

    /* Print gradient magnitude matrix */
    print_matrix("Gradient Magnitude:", grad_mag);

    /* Print binary edge image */
    print_matrix("Edge Map (threshold=128):", edge_binary);

    return 0;
}
