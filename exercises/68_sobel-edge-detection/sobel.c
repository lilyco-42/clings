/* 68_sobel-edge-detection/sobel.c — Sobel Edge Detection on PGM Images
 *
 * 任务：对固定的 8x8 灰度 PGM 图像实现 Sobel 边缘检测。
 * 图像包含垂直边、水平边、对角边和均匀区域。
 *
 * 需实现的函数：
 *   print_matrix()        — 打印矩阵 (带标题和格式对齐)
 *   convolve()            — 对图像像素做 3x3 卷积
 *   gradient_magnitude()  — 用 Sobel 算子计算梯度幅值
 *   threshold()           — 阈值二值化 (≥128→255, <128→0)
 *   main()                — 主流程：原始图像→梯度幅值→边缘二值图
 *
 * Sobel 核 GX 和 GY 已提供，保持不变。
 *
 * 知识点：图像卷积，Sobel 算子，梯度计算，阈值分割，边缘检测
 *
 * 验证：make test 对比 expected_output.txt
 */
#include <math.h>
#include <stdio.h>

#define ROWS 8
#define COLS 8
#define THRESHOLD 128

/* ─── 图像数据 (硬编码的 8x8 灰度 PGM) ───
 *
 * 此图像包含多种边缘类型：
 *   - 垂直边：行 0-2 中，列 0-1 暗 (~50)，列 2-3 亮 (~200)，
 *     在列 1→2 处形成垂直边缘。
 *   - 水平边：行 2→3 处从混合亮度过渡到全亮 (~200)，
 *     形成水平边缘。
 *   - 对角边：行 5 列 0 为暗像素，形成对角过渡。
 *   - 均匀区：行 6-7 全暗 (~50)，梯度为零。
 */
static const int IMAGE[ROWS][COLS] = {
    {50, 50, 200, 200, 50, 50, 50, 50},       {50, 50, 200, 200, 50, 50, 50, 50},
    {50, 50, 200, 200, 50, 50, 50, 50},       {200, 200, 200, 200, 200, 200, 200, 200},
    {200, 200, 200, 200, 200, 200, 200, 200}, {50, 200, 200, 200, 200, 200, 200, 200},
    {50, 50, 50, 50, 50, 50, 50, 50},         {50, 50, 50, 50, 50, 50, 50, 50},
};

/* ─── Sobel 卷积核 (已提供，保持不变) ───
 * GX: 检测垂直边缘 (水平方向梯度)
 * GY: 检测水平边缘 (垂直方向梯度)
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

/* ─── TODO 1: print_matrix ───
 * 打印一个整数矩阵，带标题。
 * 格式：先打印标题行，然后每行元素用空格分隔，每个元素占 3 位宽度 (%3d),
 *       行末换行，矩阵后额外空一行。
 * 提示：用两层 for 循环遍历行列，j>0 时先打印空格。 */
static void print_matrix(const char *title, int mat[ROWS][COLS]) {
#error TODO 1: Print the matrix — print title line, then each row with printf("%3d") and space separators. Add a blank line after the matrix.
}

/* ─── TODO 2: convolve ───
 * 对图像在像素 (row, col) 处应用 3x3 卷积核。
 * 遍历以 (row,col) 为中心的 3x3 邻域：
 *   for i = -1 to 1:
 *     for j = -1 to 1:
 *       sum += image[row+i][col+j] * kernel[i+1][j+1]
 * 返回加权和 sum。
 * 注意：调用者保证 (row,col) 不是边界像素 (有完整 3x3 邻域)。 */
static int convolve(int image[ROWS][COLS], const int kernel[3][3], int row, int col) {
#error TODO 2: Convolve — iterate over 3x3 neighborhood, multiply image pixel by kernel value, accumulate sum, return sum.
}

/* ─── TODO 3: gradient_magnitude ───
 * 用 Sobel 算子计算像素 (row,col) 处的梯度幅值。
 *   1. gx = convolve(image, GX, row, col)  — X 方向梯度
 *   2. gy = convolve(image, GY, row, col)  — Y 方向梯度
 *   3. 返回 (int)(sqrt(gx*gx + gy*gy) + 0.5)  — 四舍五入
 * 需要 <math.h>, 链接时加 -lm。 */
static int gradient_magnitude(int image[ROWS][COLS], int row, int col) {
#error TODO 3: Gradient magnitude — compute gx and gy via convolve, then return sqrt(gx*gx + gy*gy) rounded to nearest int.
}

/* ─── TODO 4: threshold ───
 * 二值化：若 value >= THRESHOLD 返回 255, 否则返回 0。
 * 用三元运算符 ? : 一行即可。 */
static int threshold(int value) {
#error TODO 4: Threshold — return 255 if value >= THRESHOLD, else 0. Use the ternary operator.
}

int main(void) {
    int grad_mag[ROWS][COLS];
    int edge_binary[ROWS][COLS];

    /* TODO 5: 主流程
     *
     * Step 1: 调用 print_matrix 打印原始图像 IMAGE。
     *
     * Step 2: 遍历所有像素 (i=0..ROWS-1, j=0..COLS-1):
     *   若 i==0 或 i==ROWS-1 或 j==0 或 j==COLS-1:
     *     grad_mag[i][j] = 0  (边界像素无完整 3x3 邻域，设为 0)
     *   否则：
     *     grad_mag[i][j] = gradient_magnitude(IMAGE, i, j)
     *
     * Step 3: 遍历所有像素，edge_binary[i][j] = threshold(grad_mag[i][j])
     *
     * Step 4: 调用 print_matrix 打印 grad_mag (标题 "Gradient Magnitude:")
     *
     * Step 5: 调用 print_matrix 打印 edge_binary (标题 "Edge Map (threshold=128):")
     *
     * 注意：IMAGE 是 const int[ROWS][COLS], 传给接受 int(*)[COLS] 的函数时
     *       需要强制转换 (int(*)[COLS])IMAGE。 */
#error TODO 5: Main flow — print original image, compute gradient magnitude for interior pixels (borders=0), threshold, print both result matrices. Cast IMAGE with (int(*)[COLS])IMAGE when passing to functions.
}
