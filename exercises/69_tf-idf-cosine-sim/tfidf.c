/* 69_tf-idf-cosine-sim — TF-IDF 文档相似度计算
 *
 * 任务：计算 3 篇文档的 TF-IDF 向量及两两余弦相似度。
 *       固定文档：
 *         D0 = "the cat sat on mat"
 *         D1 = "the dog sat on log"
 *         D2 = "cat dog ate food"
 *
 * 知识点：TF 词频 / IDF 逆文档频率 / TF-IDF 加权 / 余弦相似度 / 向量空间模型
 *
 * 验证：
 *   构建：make → 编译生成 tfidf 可执行文件 (需 -lm 链接数学库)
 *   判分/自测：clings run 或 clings watch (clings 逐行比对 stdout, clings tests 69 查看期望输出)
 */

#include <math.h>
#include <stdio.h>
#include <string.h>

/* ─── 文档内容 (已提供，不可修改) ─── */
#define N_DOCS 3
#define N_TERMS 9
#define MAX_TOKENS 5

static const char *DOCS[N_DOCS] = {"the cat sat on mat", "the dog sat on log", "cat dog ate food"};

/* 词表 (按出现顺序：the, cat, sat, on, mat, dog, log, ate, food) */
static const char *VOCAB[N_TERMS] = {"the", "cat", "sat", "on", "mat", "dog", "log", "ate", "food"};

/* ─── 学生需要实现的函数 ─── */

/* TODO 1: tokenize — 将文档 doc 按空格拆分为单词数组 tokens[]
 *
 * 参数：
 *   doc     — 输入字符串 (空格分隔，会被 strtok 原地修改)
 *   tokens[]— 输出：单词指针数组
 *   max     — tokens 数组容量
 *
 * 返回：token 数量
 *
 * 提示：用 strtok(doc, " ") 拆分，循环直到 NULL 或 max 满。
 *       第一次调用：strtok(doc, " "), 后续：strtok(NULL, " ")。
 *       tokens[n++] = token 记录每个单词。
 */
static int tokenize(char *doc, char *tokens[], int max) {
#error TODO 1: Use strtok to split doc by space into tokens[]. Return token count.
}

/* TODO 2: compute_tf — 计算一篇文档的词频 (Term Frequency)
 *
 * 参数：
 *   tokens[] — 文档的单词数组
 *   n_tokens — token 数量
 *   tf[]     — 输出：长度为 N_TERMS 的 TF 向量 (每个词的原始计数)
 *
 * 实现：
 *   1) 初始化 tf[0..N_TERMS-1] = 0
 *   2) 对每个 token, 用 strcmp 在 VOCAB 中查找匹配索引，tf[idx]++
 *
 * 提示：双重循环 — 外层遍历 tokens, 内层遍历 VOCAB 用 strcmp 匹配。
 */
static void compute_tf(char *tokens[], int n_tokens, int tf[]) {
#error TODO 2: Count each token frequency. Match against VOCAB with strcmp.
}

/* TODO 3: compute_idf — 计算逆文档频率 (Inverse Document Frequency)
 *
 * 参数：
 *   tf_matrix[N_DOCS][N_TERMS] — 所有文档的 TF 矩阵
 *   idf[]                      — 输出：长度为 N_TERMS 的 IDF 向量
 *
 * 公式：idf[t] = log(N_DOCS / df[t])
 *       其中 df[t] = 包含词 t 的文档数 (tf[d][t] > 0 即包含)
 *
 * 提示：对每个词 t, 先数 df (遍历所有文档看 tf[d][t]>0), 再算 log。
 *       使用自然对数 log() (在 math.h 中，链接 -lm)。
 *       N_DOCS 和 df 都是整数，除法前需转为 double。
 */
static void compute_idf(int tf_matrix[N_DOCS][N_TERMS], double idf[]) {
#error TODO 3: For each term t, count df then compute idf[t] = log(N_DOCS / df).
}

/* TODO 4: compute_tfidf — 计算 TF-IDF 加权矩阵
 *
 * 参数：
 *   tf_matrix[N_DOCS][N_TERMS] — TF 矩阵
 *   idf[N_TERMS]               — IDF 向量
 *   tfidf[N_DOCS][N_TERMS]     — 输出：TF-IDF 矩阵
 *
 * 公式：tfidf[d][t] = tf[d][t] * idf[t]
 *
 * 提示：双重循环遍历所有文档 d 和所有词 t, 逐元素相乘。
 */
static void compute_tfidf(int tf_matrix[N_DOCS][N_TERMS], const double idf[], double tfidf[N_DOCS][N_TERMS]) {
#error TODO 4: Multiply each tf[d][t] by idf[t] to get tfidf[d][t].
}

/* TODO 5: cosine_sim — 计算两个 TF-IDF 向量的余弦相似度
 *
 * 参数：
 *   a[] — 文档 i 的 TF-IDF 向量 (长度 N_TERMS)
 *   b[] — 文档 j 的 TF-IDF 向量 (长度 N_TERMS)
 *
 * 返回：cos(a,b) = (a·b) / (|a| * |b|)
 *
 * 实现：
 *   1) dot = Σ a[k] * b[k]          (点积)
 *   2) norm_a = sqrt(Σ a[k]²)       (L2 范数)
 *      norm_b = sqrt(Σ b[k]²)
 *   3) 若 norm_a == 0 或 norm_b == 0, 返回 0.0
 *   4) 否则返回 dot / (norm_a * norm_b)
 *
 * 提示：用 sqrt() 开方 (math.h, -lm)。注意浮点除零保护。
 */
static double cosine_sim(const double a[], const double b[]) {
#error TODO 5: Compute dot product, norms, then cosine = dot / (|a|*|b|).
}

/* TODO 6: print_matrix — 打印矩阵 (通用辅助函数)
 *
 * 参数：
 *   title       — 矩阵标题 (如 "=== Term Frequency (TF) Matrix ===")
 *   row_labels[] — 行标签 (如 {"D0","D1","D2"})
 *   col_labels[] — 列标签 (如 VOCAB), 可为 NULL
 *   data[n_rows][n_cols] — 矩阵数据
 *   n_rows, n_cols — 维度
 *   fmt         — printf 格式 (如 "%6d" 或 "%6.4f")
 *   is_double   — 1=数据是 double 数组，0=数据是 int 数组
 *
 * 提示：
 *   - 先 printf("%s\n", title) 打印标题
 *   - 若 col_labels 非 NULL, 打印表头行：先 printf("%-6s", "") 空 6 格，
 *     再 for j: printf(" %6s", col_labels[j]), 最后 printf("\n")
 *   - 遍历所有行：printf("%-6s", row_labels[i]),
 *     再 for j: printf(" "); printf(fmt, val); 最后 printf("\n")
 *   - 根据 is_double 决定如何读取 data:
 *     double: ((double*)data)[i * n_cols + j]
 *     int:    ((int*)data)[i * n_cols + j]
 */
static void print_matrix(const char *title, const char *row_labels[], const char *col_labels[], const void *data,
                         int n_rows, int n_cols, const char *fmt, int is_double) {
#error TODO 6: Print matrix with row/col labels. Handle int and double data.
}

/* TODO 7: main — 主流程
 *
 * 完整流程：
 *   1) 打印 "=== TF-IDF Document Similarity ===\n"
 *   2) 打印 "Documents:" 及 3 篇文档内容
 *   3) 对每篇文档：
 *      a) strcpy 复制 DOCS[d] 到局部缓冲区 (strtok 会修改字符串)
 *      b) 调用 tokenize() 拆分
 *      c) 调用 compute_tf() 填充 tf_matrix[d]
 *   4) 调用 compute_idf() 计算 IDF
 *   5) 调用 compute_tfidf() 计算 TF-IDF
 *   6) 对每对 (d1,d2) 调用 cosine_sim() 填充 sim[d1][d2]
 *   7) 依次打印：
 *      a) TF 矩阵 (int 格式，col_labels=VOCAB)
 *      b) IDF 向量 (含 N 说明行，再用"Term"行 + "IDF"行)
 *      c) TF-IDF 矩阵 (%.4f 格式)
 *      d) 相似度矩阵 (%.4f 格式，行/列标签均为 D0/D1/D2)
 *      e) 解释信息 (Interpretation: 三行)
 *
 * 输出格式必须逐字符准确 (clings 判分时会逐行比对程序 stdout)。
 * 用 clings tests 69 查看完整的期望输出格式。
 */
int main(void) {
#error TODO 7: Implement the full TF-IDF pipeline and print all results.
}
