## Lesson 69 — TF-IDF 文档相似度计算【信息检索实战】

### 课程任务

用纯 C 语言实现 **TF-IDF 向量空间模型**，计算 3 篇文档的加权向量和两两**余弦相似度**。

学员需要完成 7 个部分：

1. **`tokenize()`** — 用 `strtok` 将文档按空格拆分为单词数组
2. **`compute_tf()`** — 统计词频（Term Frequency），生成 TF 向量
3. **`compute_idf()`** — 计算逆文档频率（Inverse Document Frequency）
4. **`compute_tfidf()`** — 计算 TF-IDF 加权矩阵
5. **`cosine_sim()`** — 计算两个向量的余弦相似度
6. **`print_matrix()`** — 通用矩阵打印（支持 int/double，带行列标签）
7. **`main()`** — 主流程：分词 → TF → IDF → TF-IDF → 相似度矩阵 → 输出

三篇文档和词汇表已完整提供，学员只需实现算法流程。

---

### 前置知识：什么是 TF-IDF？

#### 历史背景

TF-IDF（Term Frequency - Inverse Document Frequency）由 **Karen Spärck Jones** 于 1972 年在论文《A Statistical Interpretation of Term Specificity and Its Application in Retrieval》中提出。这是信息检索领域最经典的加权技术，至今仍是搜索引擎、文档聚类、推荐系统的基石。

#### 核心思想

TF-IDF 的核心直觉非常朴素：

- **TF（词频）**：一个词在某文档中出现越频繁，对该文档越重要
- **IDF（逆文档频率）**：但若该词在很多文档中都出现（如 "the"、"a"、"is"），其区分能力就弱
- **TF-IDF = TF × IDF**：既考虑词在本文档中的重要性，又惩罚常见词，奖励稀有词

```
直觉示例:

  "the" 在 D0 和 D1 中都出现 → df=2, idf=log(3/2)≈0.41 → 低权重（区分度差）
  "mat" 仅在 D0 中出现       → df=1, idf=log(3/1)≈1.10 → 高权重（区分度好）
  "food" 仅在 D2 中出现      → df=1, idf=log(3/1)≈1.10 → 高权重（区分度好）
```

---

### 前置知识：向量空间模型（VSM）

#### 文档即向量

向量空间模型（Vector Space Model, VSM）由 Gerard Salton 在 1970 年代提出。核心思想是将文档表示为高维向量空间中的点。

```
词汇表 V = [the, cat, sat, on, mat, dog, log, ate, food]  (9维空间)

D0 = "the cat sat on mat"
  → 原始计数向量: [1, 1, 1, 1, 1, 0, 0, 0, 0]
  → TF-IDF向量:   [0.41, 0.41, 0.41, 0.41, 1.10, 0, 0, 0, 0]
                     ↑     ↑     ↑     ↑     ↑
                    the   cat   sat    on   mat

D1 = "the dog sat on log"
  → 原始计数向量: [1, 0, 1, 1, 0, 1, 1, 0, 0]
  → TF-IDF向量:   [0.41, 0, 0.41, 0.41, 0, 0.41, 1.10, 0, 0]

D2 = "cat dog ate food"
  → 原始计数向量: [0, 1, 0, 0, 0, 1, 0, 1, 1]
  → TF-IDF向量:   [0, 0.41, 0, 0, 0, 0.41, 0, 1.10, 1.10]
```

#### 二维空间可视化（简化）

虽然实际是 9 维空间，但可以想象在 2D 平面上的投影：

```
               food/ate 轴
                  ↑
                  |     ★ D2 (cat, dog, ate, food)
                  |    /
                  |   /     ← D0-D2 夹角大 (仅共享 cat)
                  |  /
                  | /
                  |/_________→ the/sat/on 轴
                 /|
                / |
               /  |  ★ D1 (the, dog, sat, on, log)
              /   |
             ★    |
            D0    |
      (the, cat, sat, on, mat)

D0-D1 夹角较小 → cos ≈ 0.26 (共享 the/sat/on)
D0-D2 夹角较大 → cos ≈ 0.07 (仅共享 cat)
D1-D2 夹角较大 → cos ≈ 0.07 (仅共享 dog)
```

---

### 算法详解

#### Step 1: Tokenize（分词）

将文档字符串按空格拆分为单词列表。

```
输入: "the cat sat on mat"
       ↓ strtok(doc, " ")
输出: tokens = ["the", "cat", "sat", "on", "mat"]
      n = 5
```

**关键点**：`strtok` 会原地修改字符串（在分隔符处插入 `\0`），所以必须先 `strcpy` 复制原字符串，否则会破坏 `const` 文档数据。

```
正确: strcpy(buf, DOCS[d]); tokenize(buf, tokens, MAX_TOKENS);
错误: tokenize((char*)DOCS[d], tokens, MAX_TOKENS);  // DOCS是const!
```

#### Step 2: Compute TF（词频统计）

对每个 token，在词汇表中查找匹配项，对应位置计数 +1。

```
算法伪代码:
  for j = 0..N_TERMS-1: tf[j] = 0          // 初始化
  for i = 0..n_tokens-1:                    // 遍历每个token
    for j = 0..N_TERMS-1:                   // 在词汇表中查找
      if strcmp(tokens[i], VOCAB[j]) == 0:
        tf[j]++
        break                                // 找到就跳出
```

**TF 矩阵（原始计数）**：

```
           the  cat  sat   on  mat  dog  log  ate food
D0           1    1    1    1    1    0    0    0    0
D1           1    0    1    1    0    1    1    0    0
D2           0    1    0    0    0    1    0    1    1
```

注意：本题使用原始计数（raw count），不做归一化。因为文档长度相近（都是 4-5 个词），且 IDF 已经提供了跨词归一化。

#### Step 3: Compute IDF（逆文档频率）

IDF 衡量一个词的"稀有度"或"信息量"。

```
公式: idf(t) = log(N / df(t))

  其中:
    N     = 文档总数 (本题 N=3)
    df(t) = 包含词 t 的文档数 (Document Frequency)
           = 统计有多少文档中 tf[d][t] > 0
```

> **本题采用的 IDF 变体（重要）**：公式中的 `log` 是**自然对数 `ln`**（对应 C 的 `log()`），且**不做平滑**，即 `idf(t) = ln(N / df(t))`。这是维基百科 [tf–idf](https://en.wikipedia.org/wiki/Tf%E2%80%93idf) 列出的标准「基础」形式，正确无误。它与 scikit-learn 默认的**平滑变体**不同——sklearn 用 `idf(t) = ln((1 + N) / (1 + df(t))) + 1`（分子分母各加 1 以避免除零、末尾 +1 使权重非零）。两者数值不同：**本题判分以上面「自然对数、无平滑」的公式为准**，若套用 sklearn 的平滑公式，输出会对不上。

**IDF 计算表**：

```
词(t)    df(t)    N/df     idf = log(N/df)    解释
─────    ────     ────     ───────────────    ──────────
the        2      1.5      log(1.5) = 0.4055  常见词，权重低
cat        2      1.5      log(1.5) = 0.4055  出现在D0和D2
sat        2      1.5      log(1.5) = 0.4055  出现在D0和D1
on         2      1.5      log(1.5) = 0.4055  出现在D0和D1
mat        1      3.0      log(3.0) = 1.0986  稀有词，权重高
dog        2      1.5      log(1.5) = 0.4055  出现在D1和D2
log        1      3.0      log(3.0) = 1.0986  稀有词，权重高
ate        1      3.0      log(3.0) = 1.0986  稀有词，权重高
food       1      3.0      log(3.0) = 1.0986  稀有词，权重高
```

**关键陷阱：整数除法**

```
错误: log(N_DOCS / df)       // N_DOCS=3, df=2 → 3/2=1 (整数除法截断!)
正确: log((double)N_DOCS / df)  // 3.0/2 = 1.5
```

若某词出现在所有文档中（df = N），则 `idf = log(1) = 0`，该词被完全忽略——这符合直觉：出现在所有文档中的词没有任何区分能力。

#### Step 4: Compute TF-IDF（加权）

将 TF 和 IDF 逐元素相乘。

```
公式: tfidf[d][t] = tf[d][t] × idf[t]

算法:
  for d = 0..N_DOCS-1:
    for t = 0..N_TERMS-1:
      tfidf[d][t] = tf_matrix[d][t] * idf[t]
```

**TF-IDF 矩阵**：

```
           the    cat    sat     on    mat    dog    log    ate   food
D0       0.4055 0.4055 0.4055 0.4055 1.0986 0.0000 0.0000 0.0000 0.0000
D1       0.4055 0.0000 0.4055 0.4055 0.0000 0.4055 1.0986 0.0000 0.0000
D2       0.0000 0.4055 0.0000 0.0000 0.0000 0.4055 0.0000 1.0986 1.0986
```

观察：

- `the` 在 D0/D1 的权重仅 0.4055（常见词被 IDF 惩罚）
- `mat` 在 D0 的权重为 1.0986（稀有词被 IDF 奖励）
- `ate` 和 `food` 在 D2 的权重为 1.0986（都是稀有词）
- D2 有两个高权重词 → D2 向量长度较大

#### Step 5: Cosine Similarity（余弦相似度）

余弦相似度度量两个向量方向的接近程度，不受向量长度影响。

```
公式: cos(a, b) = (a · b) / (|a| × |b|)

  其中:
    a · b = Σ a[k] × b[k]                     (点积)
    |a|   = sqrt(Σ a[k]²)                     (L2范数)
    |b|   = sqrt(Σ b[k]²)
```

**几何直觉**：在 9 维空间中，每篇文档是一个向量。余弦相似度 = 两向量夹角的余弦。

```
  - 夹角 0°  → cos=1   (完全同向，内容几乎一样)
  - 夹角 90° → cos=0   (正交，无共同词)
  - 夹角 180°→ cos=-1  (完全反向，TF-IDF非负时不会出现)
```

**为什么用余弦而非欧氏距离？**

```
欧氏距离受向量长度影响:
  - 长文档（词多）天然距离远
  - 短文档（词少）天然距离近
  - 两篇内容相似但长度不同的文档，欧氏距离可能很大

余弦相似度只看方向:
  - 不受文档长度影响
  - 长文档和短文档可能非常相似（方向一致）
  - 对文本相似度判断更合理
```

**算法**：

```
double cosine_sim(const double a[], const double b[]) {
    double dot = 0.0, norm_a = 0.0, norm_b = 0.0;
    for (k = 0; k < N_TERMS; k++) {
        dot    += a[k] * b[k];
        norm_a += a[k] * a[k];
        norm_b += b[k] * b[k];
    }
    if (norm_a == 0.0 || norm_b == 0.0) return 0.0;  // 零向量保护
    return dot / (sqrt(norm_a) * sqrt(norm_b));
}
```

**相似度矩阵**：

```
           D0      D1      D2
D0      1.0000  0.2645  0.0727
D1      0.2645  1.0000  0.0727
D2      0.0727  0.0727  1.0000
```

分析：

- **D0-D0, D1-D1, D2-D2 = 1.0000**：文档与自身完全相似（自相似度总是 1）
- **D0-D1 = 0.2645**：中等相似。共享 the、sat、on 三个词，但各有独有词（mat vs dog/log）
- **D0-D2 = 0.0727**：低相似。仅共享 cat
- **D1-D2 = 0.0727**：低相似。仅共享 dog
- 矩阵对称：cos(D0,D1) = cos(D1,D0)

---

### 搜索引擎中的应用

#### 现代搜索引擎的基本流程

```
┌─────────────────────────────────────────────────────┐
│                   离线索引阶段                         │
│                                                       │
│  网页爬取 → 文本提取 → 分词 → TF-IDF向量化            │
│                                    ↓                  │
│                          倒排索引存储                  │
│                          (term → doc_list)             │
└─────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────┐
│                   在线查询阶段                         │
│                                                       │
│  用户查询 "cat food"                                  │
│       ↓                                               │
│  查询向量化 (TF-IDF)                                  │
│       ↓                                               │
│  计算查询向量 与 所有文档向量 的余弦相似度             │
│       ↓                                               │
│  按相似度降序排列 → 返回 Top-K 结果                   │
└─────────────────────────────────────────────────────┘
```

#### 本例中的查询模拟

```
查询 "cat":
  - D0 有 cat → TF-IDF权重 0.4055 → 相似度 > 0
  - D1 无 cat → 相似度 = 0
  - D2 有 cat → TF-IDF权重 0.4055 → 相似度 > 0
  排名: D0 ≈ D2 > D1

查询 "the cat":
  - D0: the+cat 都有 → 高相似度
  - D1: 有 the 无 cat → 中等相似度
  - D2: 有 cat 无 the → 中等相似度
  排名: D0 > D1 ≈ D2
```

#### 局限性（了解即可）

1. **词袋模型**：忽略词序，"cat eats dog" 和 "dog eats cat" 向量完全相同
2. **词汇表外词（OOV）**：查询中的新词无法匹配
3. **同义词**："car" 和 "automobile" 被视为完全不同的词
4. **短查询问题**：查询 "the cat" 可能匹配大量文档（因为 "the" 常见）

现代搜索引擎在 TF-IDF 基础上发展出了 BM25、语言模型、神经网络嵌入等方法。

---

### 输出格式详解

程序输出 6 个部分，格式必须逐字符准确（clings 判分时会逐行比对程序 stdout；用 `clings tests 69` 可查看期望输出）。

#### Part 1: 标题和文档列表

```
=== TF-IDF Document Similarity ===

Documents:
  D0: "the cat sat on mat"
  D1: "the dog sat on log"
  D2: "cat dog ate food"
```

#### Part 2: TF 矩阵

```
=== Term Frequency (TF) Matrix ===
          the    cat    sat     on    mat    dog    log    ate   food
D0          1      1      1      1      1      0      0      0      0
D1          1      0      1      1      0      1      1      0      0
D2          0      1      0      0      0      1      0      1      1
```

格式说明：

- 列标签右对齐，宽度 6（`%6s`）
- 数据右对齐，宽度 6（`%6d`）
- 每行前有左对齐的行标签（`%-6s`）
- 建议使用 `print_matrix` 统一打印

#### Part 3: IDF 向量

```
=== Inverse Document Frequency (IDF) ===
N = 3 documents
Term      the    cat    sat     on    mat    dog    log    ate   food
IDF    0.4055 0.4055 0.4055 0.4055 1.0986 0.4055 1.0986 1.0986 1.0986
```

格式说明：

- 先打印 "N = 3 documents"
- "Term" 行：列标签（`%6s`）
- "IDF" 行：4 位小数（`%6.4f`）
- IDF 部分不使用 `print_matrix`，因为格式特殊（多了 N 说明行）

#### Part 4: TF-IDF 矩阵

```
=== TF-IDF Weighted Matrix ===
          the    cat    sat     on    mat    dog    log    ate   food
D0     0.4055 0.4055 0.4055 0.4055 1.0986 0.0000 0.0000 0.0000 0.0000
D1     0.4055 0.0000 0.4055 0.4055 0.0000 0.4055 1.0986 0.0000 0.0000
D2     0.0000 0.4055 0.0000 0.0000 0.0000 0.4055 0.0000 1.0986 1.0986
```

格式说明：数据用 `%6.4f`，建议使用 `print_matrix(..., "%6.4f", 1)`

#### Part 5: 相似度矩阵

```
=== Cosine Similarity Matrix (3x3) ===
           D0     D1     D2
D0     1.0000 0.2645 0.0727
D1     0.2645 1.0000 0.0727
D2     0.0727 0.0727 1.0000
```

格式说明：

- 行标签和列标签都是 D0/D1/D2
- 数据用 `%6.4f`
- 建议使用 `print_matrix(..., "%6.4f", 1)`

#### Part 6: 解释信息

```
Interpretation:
  D0-D1: share common words (the, sat, on) -> moderate similarity
  D0-D2: share only 'cat' -> low similarity
  D1-D2: share only 'dog' -> low similarity
```

---

### print_matrix 函数详解

`print_matrix` 是本练习的通用辅助函数，用于统一打印各种矩阵。

```
函数签名:
void print_matrix(const char *title,
                  const char *row_labels[],
                  const char *col_labels[],
                  const void *data,
                  int n_rows, int n_cols,
                  const char *fmt, int is_double)
```

**参数说明**：

| 参数         | 类型             | 说明                                      |
| ------------ | ---------------- | ----------------------------------------- |
| `title`      | `const char *`   | 矩阵标题，如 `"\n=== TF Matrix ==="`      |
| `row_labels` | `const char *[]` | 行标签数组，如 `{"D0","D1","D2"}`         |
| `col_labels` | `const char *[]` | 列标签数组，可为 `NULL`                   |
| `data`       | `const void *`   | 矩阵数据（一维扁平数组）                  |
| `n_rows`     | `int`            | 行数                                      |
| `n_cols`     | `int`            | 列数                                      |
| `fmt`        | `const char *`   | printf 格式，如 `"%6d"` 或 `"%6.4f"`      |
| `is_double`  | `int`            | 1=data 是 double 数组，0=data 是 int 数组 |

_*void* 索引计算_*：

```
错误: ((double*)data)[i][j]          // data是flat数组，不是二维数组!
正确: ((double*)data)[i * n_cols + j]  // 一维索引 = 行×列数 + 列
正确: ((int*)data)[i * n_cols + j]     // int版本同理
```

**调用示例**：

```c
// 打印 TF 矩阵 (int)
print_matrix("\n=== Term Frequency (TF) Matrix ===",
             row_labels, VOCAB,
             tf_matrix, N_DOCS, N_TERMS, "%6d", 0);

// 打印 TF-IDF 矩阵 (double)
print_matrix("\n=== TF-IDF Weighted Matrix ===",
             row_labels, VOCAB,
             tfidf, N_DOCS, N_TERMS, "%6.4f", 1);

// 打印相似度矩阵 (double, 行列标签相同)
print_matrix("\n=== Cosine Similarity Matrix (3x3) ===",
             row_labels, row_labels,   // 行列标签都用 D0/D1/D2
             sim, N_DOCS, N_DOCS, "%6.4f", 1);
```

---

### 编译与运行

#### 编译

```bash
# 正确编译（-lm 必须在源文件之后）
gcc -Wall -Wextra -std=c11 -o tfidf tfidf.c -lm

# 错误编译（缺少 -lm）
gcc -Wall -Wextra -std=c11 -o tfidf tfidf.c
# → undefined reference to 'log'
# → undefined reference to 'sqrt'
```

**为什么需要 `-lm`？** `log()` 和 `sqrt()` 定义在数学库 `libm` 中，而非标准 C 库 `libc`。链接器需要显式指定 `-lm`。

**为什么 `-lm` 必须放在源文件之后？** 链接器按从左到右的顺序处理。若 `-lm` 在前而源文件引用在后，链接器在处理 `-lm` 时还没有未解析的符号，会跳过数学库中的符号，导致链接失败。

#### 运行

```bash
./tfidf
```

#### 测试

```bash
# 构建：make（等价于 make all）
# 自测/判分：clings run 69   或在 clings watch 下自动运行
# 查看期望输出：clings tests 69
```

clings 会用 `make` 构建后运行 `tfidf`，捕获其 stdout 与内置测试用例逐行比对，一致即通过。

---

### 常见错误

#### 1. 忘记链接 -lm

```
错误信息: undefined reference to 'log' / 'sqrt'
解决: gcc ... tfidf.c -lm  （-lm 必须在最后）
```

#### 2. strtok 修改原字符串

```
错误: tokenize((char*)DOCS[d], tokens, MAX_TOKENS);
      // DOCS 是 const char*[]，strtok 会写入 '\0' 导致崩溃或 UB

正确: char buf[64];
      strcpy(buf, DOCS[d]);
      tokenize(buf, tokens, MAX_TOKENS);
```

#### 3. IDF 计算中整数除法

```
错误: idf[t] = log(N_DOCS / df);
      // N_DOCS=3, df=2 → 3/2=1 (整数除法截断!) → log(1)=0

正确: idf[t] = log((double)N_DOCS / df);
      // 3.0/2 = 1.5 → log(1.5) ≈ 0.4055
```

#### 4. 余弦相似度除零

```
若某文档的 TF-IDF 向量全为零（例如空文档），范数为 0。
dot / (0 * 0) → NaN

必须检查: if (norm_a == 0.0 || norm_b == 0.0) return 0.0;
```

#### 5. print_matrix 中 void* 索引错误

```
错误: ((double*)data)[i][j]
      // data 是扁平一维数组，不是二维数组

正确: ((double*)data)[i * n_cols + j]
```

#### 6. 输出格式细节

```
列标签: printf(" %6s", label)     ← 前面有空格
数据:   printf(" "); printf(fmt, val)  ← 先打空格再打数值
行标签: printf("%-6s", label)     ← 左对齐，宽度6
标题:   printf("%s\n", title)     ← 无冒号，直接换行
```

#### 7. 自相似度浮点误差

```
文档与自身的余弦相似度理论上为 1.0。
浮点运算可能产生 0.9999...，但用 %.4f 打印会四舍五入为 1.0000。
```

#### 8. strcmp 返回值混淆

```
错误: if (strcmp(tokens[i], VOCAB[j]))  // 非零为真，逻辑反了!
正确: if (strcmp(tokens[i], VOCAB[j]) == 0)  // 返回0表示相等
```

---

### 讨论问题

1. **为什么用余弦相似度而非欧氏距离？** 文档长度不同时，余弦只看方向，不受长度影响。两篇内容相似但长度差很多的文档，欧氏距离可能很大但余弦接近 1。

2. **如果 "the" 出现在所有 3 篇文档中会怎样？** df=3, idf=log(3/3)=0。该词在 TF-IDF 矩阵中全为 0，对相似度无贡献——这是 IDF 的核心价值。

3. **为什么 D0-D1 的相似度（0.2645）远大于 D0-D2（0.0727）？** D0 和 D1 共享 3 个词（the, sat, on），而 D0 和 D2 仅共享 1 个词（cat）。共享词越多，向量方向越接近。

4. **如果增加一篇文档 D3 = "the cat sat on the mat"（the 出现两次），TF-IDF 矩阵会如何变化？** TF("the", D3)=2, 其他词不变。但 IDF 会变化（因为 N 从 3 变为 4，所有 df 可能变化），需要重新计算所有值。

5. **TF-IDF 的局限性是什么？** 词袋模型忽略词序和语义；无法处理同义词（"car" vs "automobile"）；对短文本效果差；IDF 是静态的（基于语料库），新词需要重新计算。

---

### 参考资料

- Salton, G., & Buckley, C. (1988). Term-weighting approaches in automatic text retrieval. _Information Processing & Management_, 24(5), 513-523.
- Spärck Jones, K. (1972). A statistical interpretation of term specificity and its application in retrieval. _Journal of Documentation_, 28(1), 11-21.
- Manning, C. D., Raghavan, P., & Schütze, H. (2008). _Introduction to Information Retrieval_. Cambridge University Press. Chapter 6.
- Wikipedia: [tf–idf](https://en.wikipedia.org/wiki/Tf%E2%80%93idf)
- Wikipedia: [Cosine similarity](https://en.wikipedia.org/wiki/Cosine_similarity)
- Wikipedia: [Vector space model](https://en.wikipedia.org/wiki/Vector_space_model)
