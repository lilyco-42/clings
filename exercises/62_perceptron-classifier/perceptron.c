/* 62_perceptron-classifier — 感知机二分类器
 *
 * 任务：实现单层感知机 (Perceptron) 学习 AND 逻辑函数，然后演示 XOR 的线性不可分。
 *
 * Part A — AND (线性可分):
 *   数据集 4 个点：(0,0)→-1  (0,1)→-1  (1,0)→-1  (1,1)→+1
 *   固定学习率 α = 0.1, 初始权重 w = {0, 0}, 偏置 b = 0。
 *   最多训练 100 轮，若一整轮无误分类则提前停止。
 *
 * Part B — XOR (线性不可分):
 *   数据集 4 个点：(0,0)→-1  (0,1)→+1  (1,0)→+1  (1,1)→-1
 *   同样 α=0.1, w={0,0}, b=0, max_epoch=100。
 *   演示感知机永不收敛（每轮总有 2 个误分类点），
 *   说明单层感知机的根本局限 — 这直接导致了第一次 AI 寒冬。
 *
 * 对每个误分类点 (x, y), 更新规则：
 *   w = w + α * y * x
 *   b = b + α * y
 *
 * 知识点：线性分类 / 感知机收敛定理 / 线性不可分 (XOR 问题) /
 *         浮点误差对 sign(0) 的影响 / 决策边界参数化
 *
 * 验证：
 *   make  → 编译生成 perceptron 可执行文件
 *   make test  → 运行并与 expected_output.txt 比对
 */

#include <stdio.h>

/* ─── 数据集：AND 逻辑的 4 个点 (已提供，不可修改) ─── */
#define N_POINTS 4
#define N_FEATURES 2

static const double X_AND[N_POINTS][N_FEATURES] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};

static const double Y_AND[N_POINTS] = {-1.0, -1.0, -1.0, 1.0};

/* ─── 数据集：XOR 逻辑的 4 个点 (已提供，不可修改) ─── */
static const double X_XOR[N_POINTS][N_FEATURES] = {{0.0, 0.0}, {0.0, 1.0}, {1.0, 0.0}, {1.0, 1.0}};

static const double Y_XOR[N_POINTS] = {-1.0, 1.0, 1.0, -1.0};

/* 超参数 (已提供，不可修改) */
static const double ALPHA = 0.1;
static const int MAX_EPOCHS = 100;

/* TODO 1: 实现点积 dot(w, b, x) = w0*x0 + w1*x1 + b
 *
 * 参数：
 *   w[] — 权重向量 (长度 N_FEATURES = 2)
 *   b   — 偏置 (标量)
 *   x[] — 输入特征向量 (长度 N_FEATURES = 2)
 *
 * 返回：标量 w·x + b
 */
static double dot(const double w[], double b, const double x[]) {
#error TODO 1: Compute dot product w0*x0 + w1*x1 + b. Loop over N_FEATURES.
}

/* TODO 2: 实现符号函数 sign(val)
 *
 * 返回 +1.0 如果 val >= 0.0, 否则返回 -1.0。
 * 注意：在 IEEE 754 中 -0.0 >= 0.0 为 true (即 sign(-0.0) = +1.0)。
 */
static double sign(double val) {
#error TODO 2: Return +1.0 if val >= 0.0, else -1.0.
}

/* TODO 3: 实现预测函数 predict(w, b, x)
 *
 * 返回 sign(dot(w, b, x)) — 即对点 x 的预测标签 (+1 或 -1)。
 * 直接调用 dot() 和 sign() 即可，一行代码。
 */
static double predict(const double w[], double b, const double x[]) {
#error TODO 3: Return sign(dot(w, b, x)). One line.
}

/* TODO 4: 实现单轮训练 train_one_epoch(w, &b, X_data, Y_data)
 *
 * 遍历全部 N_POINTS 个点，对每个点：
 *   1) 用 predict() 计算预测标签
 *   2) 若预测 != 真实标签 Y_data[i], 则为误分类，执行更新：
 *        w[j] += ALPHA * Y_data[i] * X_data[i][j]   (对每个特征 j)
 *        *b   += ALPHA * Y_data[i]
 *      mistakes 计数器加 1。
 *
 * 返回本轮误分类次数 mistakes。
 * 注意：w 和 b 通过指针传入，更新直接修改原值。
 *       X_data/Y_data 通过参数传入，以便复用于不同数据集。
 */
static int train_one_epoch(double w[], double *b, const double X_data[][N_FEATURES], const double Y_data[]) {
#error TODO 4: Iterate N_POINTS, predict, compare with Y_data[i], update on mismatch.
}

/* TODO 5: 实现权重/偏置打印 print_weights(w, b)
 *
 * 按格式 "w = {%.4f, %.4f}, b = %.4f" 打印 w[0], w[1], b。
 * 注意：小数点后固定 4 位，花括号内有空格。
 */
static void print_weights(const double w[], double b) {
#error TODO 5: printf("w = {%.4f, %.4f}, b = %.4f", w[0], w[1], b);
}

/* TODO 6: 实现 AND 训练主流程 (main 前半部分)
 *
 * 1) 打印标题 "=== Perceptron Binary Classifier (AND logic) ===\n\n"
 * 2) 打印数据集描述行和学习率/最大轮数
 * 3) 打印初始权重
 * 4) 循环 MAX_EPOCHS 轮：
 *    a) 打印 "--- Epoch %d ---\n"
 *    b) 打印 Before 权重
 *    c) 调用 train_one_epoch(w, &b, X_AND, Y_AND)
 *    d) 打印 After 权重
 *    e) 遍历 4 个点，打印每个点的分类结果 (预测值 + ✓/✗)
 *    f) 若 mistakes==0, 打印收敛信息并 break
 * 5) 打印 "=== Final Decision Boundary ===" 及决策边界方程
 * 6) 将决策边界化为 x1 = slope * x0 + intercept 形式
 * 7) 打印收敛轮数
 *
 * 输出格式必须与 expected_output.txt 逐字符一致。
 * 参考 expected_output.txt 了解完整输出格式。
 */

/* TODO 7: 实现 XOR 训练段 (main 后半部分)
 *
 * 在 AND 训练完成后，重置权重 w={0,0}, b=0。
 * 打印 "=== Perceptron Binary Classifier (XOR logic) ===\n\n"
 * 以及 XOR 数据集描述。
 * 同样循环训练最多 100 轮：
 *   - 前 5 轮逐轮打印完整信息
 *   - 之后每 10 轮打印一次（epoch 10, 20, 30, ... 100）
 *   - 中间轮次用紧凑摘要行替代：
 *     "  ... (epochs 6–9: oscillating, 2 mistakes each) ..."
 *     "  ... (epochs 11–19: oscillating, 2 mistakes each) ..."
 * 100 轮后仍未收敛，打印警告信息和线性不可分的说明。
 * 最后打印决策边界和 "Did not converge" 信息。
 *
 * 同样参考 expected_output.txt 了解完整输出格式。
 */
int main(void) {
#error TODO 6 & 7: Implement AND training + XOR demonstration as per expected_output.txt.
}
