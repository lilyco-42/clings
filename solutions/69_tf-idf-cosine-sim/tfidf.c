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
 *   make  → 编译生成 tfidf 可执行文件 (需 -lm 链接数学库)
 *   make test  → 运行并与 expected_output.txt 比对
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

/* ─── 工具函数 ─── */

/* tokenize: 将文档 doc 按空格拆分为单词数组 tokens[], 返回 token 数量
 *
 * 参数：
 *   doc     — 输入字符串 (空格分隔的单词)
 *   tokens[]— 输出：单词指针数组 (指向 doc 内部，原地修改)
 *   max     — tokens 数组容量
 *
 * 返回：token 数量
 *
 * 实现：用 strtok 拆分，分割符为空格。
 *       注意：strtok 会修改输入字符串，所以需要传入可修改的副本。
 */
static int tokenize(char *doc, char *tokens[], int max) {
    int n = 0;
    char *token = strtok(doc, " ");
    while (token != NULL && n < max) {
        tokens[n++] = token;
        token = strtok(NULL, " ");
    }
    return n;
}

/* compute_tf: 计算一篇文档的词频 (Term Frequency)
 *
 * 参数：
 *   tokens[] — 文档的单词数组
 *   n_tokens — token 数量
 *   tf[]     — 输出：长度为 N_TERMS 的 TF 向量 (每个词的原始计数)
 *
 * 实现：对每个 token, 在 VOCAB 中查找匹配的索引，tf[idx]++。
 *       用 strcmp 比较字符串。
 */
static void compute_tf(char *tokens[], int n_tokens, int tf[]) {
    int i, j;
    /* 初始化 TF 向量为 0 */
    for (j = 0; j < N_TERMS; j++) {
        tf[j] = 0;
    }
    /* 统计每个 token */
    for (i = 0; i < n_tokens; i++) {
        for (j = 0; j < N_TERMS; j++) {
            if (strcmp(tokens[i], VOCAB[j]) == 0) {
                tf[j]++;
                break;
            }
        }
    }
}

/* compute_idf: 计算逆文档频率 (Inverse Document Frequency)
 *
 * 参数：
 *   tf_matrix[N_DOCS][N_TERMS] — 所有文档的 TF 矩阵
 *   idf[]                      — 输出：长度为 N_TERMS 的 IDF 向量
 *
 * 公式：idf[t] = log(N_DOCS / df[t])
 *       其中 df[t] = 包含词 t 的文档数 (TF > 0 即为包含)
 *
 * 使用自然对数 log() (math.h), 编译时需链接 -lm。
 */
static void compute_idf(int tf_matrix[N_DOCS][N_TERMS], double idf[]) {
    int d, t;
    for (t = 0; t < N_TERMS; t++) {
        int df = 0;
        for (d = 0; d < N_DOCS; d++) {
            if (tf_matrix[d][t] > 0) {
                df++;
            }
        }
        idf[t] = log((double)N_DOCS / df);
    }
}

/* compute_tfidf: 计算 TF-IDF 加权矩阵
 *
 * 参数：
 *   tf_matrix[N_DOCS][N_TERMS] — TF 矩阵
 *   idf[N_TERMS]               — IDF 向量
 *   tfidf[N_DOCS][N_TERMS]     — 输出：TF-IDF 矩阵
 *
 * 公式：tfidf[d][t] = tf[d][t] * idf[t]
 */
static void compute_tfidf(int tf_matrix[N_DOCS][N_TERMS], const double idf[], double tfidf[N_DOCS][N_TERMS]) {
    int d, t;
    for (d = 0; d < N_DOCS; d++) {
        for (t = 0; t < N_TERMS; t++) {
            tfidf[d][t] = tf_matrix[d][t] * idf[t];
        }
    }
}

/* cosine_sim: 计算两个 TF-IDF 向量的余弦相似度
 *
 * 参数：
 *   a[] — 文档 i 的 TF-IDF 向量 (长度 N_TERMS)
 *   b[] — 文档 j 的 TF-IDF 向量 (长度 N_TERMS)
 *
 * 返回：cos(a,b) = (a·b) / (|a| * |b|)
 *
 * 实现：
 *   1) dot = Σ a[k] * b[k]
 *   2) norm_a = sqrt(Σ a[k]²), norm_b = sqrt(Σ b[k]²)
 *   3) 若 norm_a == 0 或 norm_b == 0, 返回 0.0 (零向量无方向)
 *   4) 否则返回 dot / (norm_a * norm_b)
 */
static double cosine_sim(const double a[], const double b[]) {
    int k;
    double dot = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (k = 0; k < N_TERMS; k++) {
        dot += a[k] * b[k];
        norm_a += a[k] * a[k];
        norm_b += b[k] * b[k];
    }
    if (norm_a == 0.0 || norm_b == 0.0) {
        return 0.0;
    }
    return dot / (sqrt(norm_a) * sqrt(norm_b));
}

/* print_matrix: 打印矩阵 (带行/列标签)
 *
 * 参数：
 *   title   — 矩阵标题
 *   row_labels[] — 行标签数组 (长度 n_rows)
 *   col_labels[] — 列标签数组 (长度 n_cols), 可为 NULL
 *   data[n_rows][n_cols] — 数据矩阵 (int 或 double, 通过 fmt 控制)
 *   n_rows, n_cols — 矩阵维度
 *   fmt     — printf 格式字符串 (如 "%d" 或 "%.4f")
 *   is_double — 1 表示 data 是 double*, 0 表示 int*
 */
static void print_matrix(const char *title, const char *row_labels[], const char *col_labels[], const void *data,
                         int n_rows, int n_cols, const char *fmt, int is_double) {
    int i, j;
    printf("%s\n", title);
    /* 列头 */
    if (col_labels != NULL) {
        printf("%-6s", "");
        for (j = 0; j < n_cols; j++) {
            printf(" %6s", col_labels[j]);
        }
        printf("\n");
    }
    /* 数据行 */
    for (i = 0; i < n_rows; i++) {
        printf("%-6s", row_labels[i]);
        for (j = 0; j < n_cols; j++) {
            if (is_double) {
                double val = ((double *)data)[i * n_cols + j];
                printf(" ");
                printf(fmt, val);
            } else {
                int val = ((int *)data)[i * n_cols + j];
                printf(" ");
                printf(fmt, val);
            }
        }
        printf("\n");
    }
}

int main(void) {
    int d, t;
    int tf_matrix[N_DOCS][N_TERMS];
    double idf[N_TERMS];
    double tfidf[N_DOCS][N_TERMS];
    double sim[N_DOCS][N_DOCS];
    char doc_buf[N_DOCS][64];
    char *tokens[MAX_TOKENS];
    int n_tokens;

    const char *row_labels_doc[N_DOCS] = {"D0", "D1", "D2"};

    printf("=== TF-IDF Document Similarity ===\n");
    printf("\nDocuments:\n");
    for (d = 0; d < N_DOCS; d++) {
        printf("  D%d: \"%s\"\n", d, DOCS[d]);
    }

    /* Step 1: Tokenize & compute TF for each document */
    for (d = 0; d < N_DOCS; d++) {
        strcpy(doc_buf[d], DOCS[d]);
        n_tokens = tokenize(doc_buf[d], tokens, MAX_TOKENS);
        compute_tf(tokens, n_tokens, tf_matrix[d]);
    }

    /* Step 2: Compute IDF */
    compute_idf(tf_matrix, idf);

    /* Step 3: Compute TF-IDF */
    compute_tfidf(tf_matrix, idf, tfidf);

    /* Step 4: Compute pairwise cosine similarity */
    for (d = 0; d < N_DOCS; d++) {
        for (int d2 = 0; d2 < N_DOCS; d2++) {
            sim[d][d2] = cosine_sim(tfidf[d], tfidf[d2]);
        }
    }

    /* ─── Output ─── */

    print_matrix("\n=== Term Frequency (TF) Matrix ===", row_labels_doc, VOCAB, tf_matrix, N_DOCS, N_TERMS, "%6d", 0);

    printf("\n=== Inverse Document Frequency (IDF) ===\n");
    printf("N = %d documents\n", N_DOCS);
    printf("%-6s", "Term");
    for (t = 0; t < N_TERMS; t++) {
        printf(" %6s", VOCAB[t]);
    }
    printf("\n");
    printf("%-6s", "IDF");
    for (t = 0; t < N_TERMS; t++) {
        printf(" %6.4f", idf[t]);
    }
    printf("\n");

    print_matrix("\n=== TF-IDF Weighted Matrix ===", row_labels_doc, VOCAB, tfidf, N_DOCS, N_TERMS, "%6.4f", 1);

    print_matrix("\n=== Cosine Similarity Matrix (3x3) ===", row_labels_doc, row_labels_doc, sim, N_DOCS, N_DOCS,
                 "%6.4f", 1);

    printf("\nInterpretation:\n");
    printf("  D0-D1: share common words (the, sat, on) -> moderate similarity\n");
    printf("  D0-D2: share only 'cat' -> low similarity\n");
    printf("  D1-D2: share only 'dog' -> low similarity\n");

    return 0;
}
