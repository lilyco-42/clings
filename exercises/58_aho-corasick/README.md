# Aho-Corasick 多模式匹配 — Trie + 失败链接的自动机

## 1. 课程任务

实现 Aho-Corasick (AC 自动机) 多模式字符串匹配算法，在固定模式集 `{"he", "she", "his", "hers"}` 和文本 `"ushers"` 上运行。完成六个核心任务：

- **TODO 1**: 实现 `new_node()` — 创建新 Trie 节点，初始化 children/fail/output
- **TODO 2**: 实现 `insert()` — 将模式串插入 Trie 树
- **TODO 3**: 实现 `bfs_build_fail()` — BFS 层序遍历构建失败链接 (Failure Link)
- **TODO 4**: 实现 `print_trie()` — 打印每个节点的子节点、失败链接、输出模式
- **TODO 5**: 实现 `search()` — 扫描文本，逐步输出状态转移和匹配结果
- **TODO 6**: 补全 `main()` — 串联上述步骤并输出最终匹配汇总

验证方式：

- **构建**：`make`（编译生成可执行文件）
- **判分 / 自测**：`clings run 58` 或 `clings watch`（自动比对标准输出）
- **查看期望输出**：`clings tests 58`

---

## 2. 前置知识

### 2.1 单模式匹配回顾

在进入多模式匹配之前，先回顾三种单模式匹配方法：

| 算法        | 预处理时间 | 匹配时间               | 核心思想                            |
| ----------- | ---------- | ---------------------- | ----------------------------------- |
| 暴力匹配    | O(1)       | O(n×m)                 | 双重循环滑动窗口逐字符比较          |
| KMP         | O(m)       | O(n)                   | 前缀函数 + 失败回退，利用已匹配信息 |
| Boyer-Moore | O(m+σ)     | O(n) 平均，O(n×m) 最坏 | 从右向左比较 + 坏字符/好后缀规则    |

其中 n = 文本长度，m = 单个模式长度，σ = 字母表大小。

### 2.2 多模式匹配问题

**问题**: 给定一个文本 T 和 k 个模式 P₁, P₂, ..., Pₖ，找出每个模式在 T 中的所有出现位置。

**暴力方法**: 对每个模式分别运行一次单模式匹配 → 总时间 O(k×n×m)（暴力）或 O(k×n)（KMP）。

**AC 自动机**: 将所有模式构建到一个 Trie 自动机中，一次扫描即可匹配所有模式 → **O(n + M + z)**，其中 M = 所有模式总长度，z = 匹配数量。

```
比喻: KMP 是「一个人拿着一张通缉令找人」，
      AC自动机是「多张通缉令叠在一起同时比对」——
      扫一遍文本就能找到所有目标。
```

### 2.3 Trie (前缀树 / 字典树)

Trie 是一种树形数据结构，用于高效存储和检索字符串集合。每个节点代表一个前缀，从根到某节点的路径上的字符拼接即为该节点表示的前缀。

```
Trie 的核心性质:
- 根节点不存储字符（空字符串）
- 每条边标记一个字符
- 从根到节点 v 的路径上的字符拼接 = 节点 v 表示的前缀
- 终止节点标记了在该节点结束的模式串
```

### 2.4 失败链接 (Failure Link)

失败链接是 AC 自动机的核心创新。对于 Trie 中的每个节点 v，其失败链接指向另一个节点 f，使得 f 表示的前缀是 v 表示的前缀的**最长真后缀**（同时也是某个模式的前缀）。

```
直觉理解:
  在 KMP 中，匹配失败时根据前缀函数回退。
  在 AC自动机中，匹配失败时沿失败链接跳转到另一个Trie节点，
  该节点代表「当前已匹配字符串的最长可用后缀」。

  失败链接 = KMP 前缀函数在 Trie 上的推广。
```

**关键性质**:

1. 根节点的 fail = 0（自身）
2. 根节点的直接子节点的 fail = 0（根）
3. 对于节点 v（父 u，边字符 c）：沿 u.fail 链向上找第一个有 c 子节点的祖先 f。若找到，v.fail = f 的 c 子节点；否则 v.fail = 0
4. 失败链接构成的图是一棵树，根为 0

### 2.5 BFS 构建失败链接

失败链接**必须**用 BFS（广度优先搜索）构建，不能用 DFS。原因：

```
BFS 保证: 当处理深度为 d 的节点时，深度 ≤ d 的所有节点的
失败链接已经计算完毕。

DFS 则可能先访问到深层节点，此时其父节点的失败链接可能
尚未计算，导致错误。
```

### 2.6 输出合并

AC 自动机的另一个关键机制：将失败链上的输出模式合并到当前节点。

```
当节点 v 有输出模式 P₁, P₂... 且 v.fail 有输出模式 Q₁, Q₂... 时:
  v 的最终输出 = P₁, P₂... + Q₁, Q₂...

这是因为: 如果文本匹配到了 v 表示的前缀，那么该前缀的所有
后缀如果也是模式串，也应该被报告。
```

---

## 3. 算法详解

### 3.1 本题 Trie 结构图

将 4 个模式 `{"he", "she", "his", "hers"}` 插入 Trie 后的结构：

```
                        ┌───────────────────────┐
                        │        Node 0          │
                        │      (root, "")        │
                        │  children: h→1, s→3    │
                        │  fail: 0               │
                        └───────┬───┬───────────┘
                               /     \
                          h  /       \  s
                             /         \
              ┌──────────────┐      ┌──────────────┐
              │   Node 1     │      │   Node 3     │
              │   ("h")      │      │   ("s")      │
              │ children:    │      │ children:    │
              │   e→2, i→6   │      │   h→4        │
              │ fail: 0      │      │ fail: 0      │
              └──┬────┬──────┘      └──────┬───────┘
                /      \                   /
           e  /        \  i           h  /
             /          \               /
    ┌──────────┐   ┌──────────┐   ┌──────────┐
    │ Node 2   │   │ Node 6   │   │ Node 4   │
    │ ("he")   │   │ ("hi")   │   │ ("sh")   │
    │ child:   │   │ child:   │   │ child:   │
    │  r→8     │   │  s→7     │   │  e→5     │
    │ fail: 0  │   │ fail: 0  │   │ fail: 1  │
    │out:["he"]│   │          │   │          │
    └──┬───────┘   └──┬───────┘   └──┬───────┘
       |              |              |
    r  |           s  |           e  |
       |              |              |
    ┌──────────┐   ┌──────────┐   ┌──────────────┐
    │ Node 8   │   │ Node 7   │   │   Node 5     │
    │ ("her")  │   │ ("his")  │   │  ("she")     │
    │ child:   │   │ children:│   │ children: {} │
    │  s→9     │   │  {}      │   │ fail: 2      │
    │ fail: 0  │   │ fail: 3  │   │out:["she",   │
    └──┬───────┘   │out:["his"]│   │     "he"]    │
       |           └──────────┘   └──────────────┘
    s  |
       |
    ┌──────────┐
    │ Node 9   │
    │ ("hers") │
    │ children:│
    │  {}      │
    │ fail: 3  │
    │out:["hers"]│
    └──────────┘
```

共 10 个节点 (Node 0 ~ Node 9)。

### 3.2 失败链接构建过程

以本题为例，逐步展示 BFS 构建失败链接的每一步：

```
初始状态: 所有 fail = 0

Step 1: 根节点的直接子节点入队
  queue = [1, 3]
  fail[1] = 0, fail[3] = 0

Step 2: 处理 Node 1 ("h"), 父=0
  子节点: e→2, i→6
  - Node 2 (h→e): fail[1]=0, 0无子'e' → fail[2]=0
  - Node 6 (h→i): fail[1]=0, 0无子'i' → fail[6]=0
  queue = [3, 2, 6]

Step 3: 处理 Node 3 ("s"), 父=0
  子节点: h→4
  - Node 4 (s→h): fail[3]=0, 0有子'h'→1 → fail[4]=1
  queue = [2, 6, 4]

Step 4: 处理 Node 2 ("he"), 父=1
  子节点: r→8
  - Node 8 (he→r): fail[2]=0, 0无子'r' → fail[8]=0
  queue = [6, 4, 8]

Step 5: 处理 Node 6 ("hi"), 父=1
  子节点: s→7
  - Node 7 (hi→s): fail[6]=0, 0有子's'→3 → fail[7]=3
  输出合并: fail[7]=3, Node 3无输出 → Node 7输出=["his"]
  queue = [4, 8, 7]

Step 6: 处理 Node 4 ("sh"), 父=3, fail=1
  子节点: e→5
  - Node 5 (sh→e): fail[4]=1, 1有子'e'→2 → fail[5]=2
  输出合并: fail[5]=2, Node 2输出=["he"]
           → Node 5输出=["she", "he"]
  queue = [8, 7, 5]

Step 7: 处理 Node 8 ("her"), 父=2
  子节点: s→9
  - Node 9 (her→s): fail[8]=0, 0有子's'→3 → fail[9]=3
  输出合并: fail[9]=3, Node 3无输出 → Node 9输出=["hers"]
  queue = [7, 5, 9]

Step 8-10: 处理 Node 7, 5, 9 — 均无子节点, BFS结束
```

**最终失败链接汇总**:

```
┌──────┬──────┬────────────────────────────┐
│ Node │ fail │  原因                       │
├──────┼──────┼────────────────────────────┤
│  0   │  0   │ 根节点自身                  │
│  1   │  0   │ 根的直接子节点              │
│  2   │  0   │ fail[1]=0, 0无'e'子节点    │
│  3   │  0   │ 根的直接子节点              │
│  4   │  1   │ fail[3]=0, 0有'h'→1        │
│  5   │  2   │ fail[4]=1, 1有'e'→2        │
│  6   │  0   │ fail[1]=0, 0无'i'子节点    │
│  7   │  3   │ fail[6]=0, 0有's'→3        │
│  8   │  0   │ fail[2]=0, 0无'r'子节点    │
│  9   │  3   │ fail[8]=0, 0有's'→3        │
└──────┴──────┴────────────────────────────┘
```

### 3.3 扫描文本过程追踪

文本 `"ushers"` (索引 0~5: u s h e r s)：

```
位置  字符  操作
─────────────────────────────────────────────────────────
  0    u    cur=0, 0无子'u' → cur保持0
           输出: 无

  1    s    cur=0, 0有子's'→3 → cur=3
           输出: Node 3无输出

  2    h    cur=3, 3有子'h'→4 → cur=4
           输出: Node 4无输出

  3    e    cur=4, 4有子'e'→5 → cur=5
           输出: Node 5 outputs=["she","he"]
           → "she" 起始=3-3+1=1, 结束=3
           → "he"  起始=3-2+1=2, 结束=3

  4    r    cur=5, 5无子'r'
           沿fail[5]=2, 2有子'r'→8 → cur=8
           输出: Node 8无输出

  5    s    cur=8, 8有子's'→9 → cur=9
           输出: Node 9 outputs=["hers"]
           → "hers" 起始=5-4+1=2, 结束=5
```

### 3.4 最终匹配结果

```
┌──────────┬──────────┬──────────┬──────────┐
│ 模式名   │ 起始位置  │ 结束位置  │ 发现方式  │
├──────────┼──────────┼──────────┼──────────┤
│ "she"    │    1     │    3     │ 直接匹配   │
│ "he"     │    2     │    3     │ 失败链接   │
│ "hers"   │    2     │    5     │ 直接匹配   │
└──────────┴──────────┴──────────┴──────────┘
```

注意：`"he"` 是通过失败链接的**输出合并**发现的——文本扫描到 `e` 时，cur=5（对应前缀 "she"），而 fail[5]=2 输出 "he"。这体现了 AC 自动机「一次扫描发现所有匹配（包括重叠模式）」的能力。

---

## 4. 代码结构详解

### 4.1 数据结构

```
TrieNode {
    int children[26];       // 26个子节点索引, -1表示无
    int fail;               // 失败链接目标节点索引
    int output_count;       // 该节点输出的模式数量
    char outputs[4][10];    // 输出模式名数组
}

全局: nodes[30] — 预分配最多30个节点
      node_count — 已创建的节点数
```

### 4.2 关键函数调用关系

```
main()
  ├─ new_node()           // TODO 1: 初始化根节点
  ├─ insert() × 4         // TODO 2: 插入4个模式串
  ├─ bfs_build_fail()     // TODO 3: BFS构建失败链接
  ├─ print_trie()         // TODO 4: 打印Trie结构
  ├─ search()             // TODO 5: 扫描文本
  └─ (手动扫描+汇总)      // TODO 6: 输出最终匹配
```

### 4.3 insert() 算法伪代码

```
insert(pattern):
    cur = 0                           // 从根开始
    for each char ch in pattern:
        c = ch - 'a'                  // 字符转索引
        if nodes[cur].children[c] == -1:
            nodes[cur].children[c] = new_node()
        cur = nodes[cur].children[c]
    // 在终止节点记录输出
    oc = nodes[cur].output_count
    strcpy(nodes[cur].outputs[oc], pattern)
    nodes[cur].output_count++
```

### 4.4 bfs_build_fail() 算法伪代码

```
bfs_build_fail():
    queue[MAX_NODES], head=0, tail=0

    // Step 1: 根的直接子节点入队
    for c in 0..25:
        child = nodes[0].children[c]
        if child != -1:
            nodes[child].fail = 0
            queue[tail++] = child

    // Step 2: BFS主循环
    while head < tail:
        cur = queue[head++]

        for c in 0..25:
            child = nodes[cur].children[c]
            if child == -1: continue

            // 计算child的失败链接
            f = nodes[cur].fail
            while f != 0 && nodes[f].children[c] == -1:
                f = nodes[f].fail
            if nodes[f].children[c] != -1 && nodes[f].children[c] != child:
                nodes[child].fail = nodes[f].children[c]
            else:
                nodes[child].fail = 0

            // 输出合并
            fail_node = nodes[child].fail
            for i in 0..nodes[fail_node].output_count-1:
                copy nodes[fail_node].outputs[i] to child's outputs
                nodes[child].output_count++

            queue[tail++] = child
```

### 4.5 search() 算法伪代码

```
search(text):
    cur = 0
    for i in 0..strlen(text)-1:
        c = text[i] - 'a'

        // 沿fail链找匹配路径
        while cur != 0 && nodes[cur].children[c] == -1:
            cur = nodes[cur].fail
        if nodes[cur].children[c] != -1:
            cur = nodes[cur].children[c]

        // 打印当前状态
        print "Step i: char='X' -> node cur"

        // 打印所有匹配
        for each output in nodes[cur].outputs:
            start_pos = i - strlen(output) + 1
            print "match: output@start_pos"
```

---

## 5. 算法复杂度对比

### 5.1 时间复杂度

| 算法          | 预处理   | 匹配 (k 个模式) | 总计           |
| ------------- | -------- | --------------- | -------------- |
| **暴力匹配**  | O(1)     | O(k × n × m)    | O(k × n × m)   |
| **KMP × k**   | O(k × m) | O(k × n)        | O(k × (n+m))   |
| **AC 自动机** | O(M × σ) | O(n + z)        | O(M×σ + n + z) |

其中：

- n = 文本长度
- m = 单个模式平均长度
- M = 所有模式总长度 = Σ|Pᵢ|
- σ = 字母表大小 (本题 σ=26)
- z = 匹配总数
- k = 模式数量

### 5.2 空间复杂度

| 算法      | 空间                         |
| --------- | ---------------------------- |
| 暴力匹配  | O(1)                         |
| KMP × k   | O(k × m) (存储 k 个前缀函数) |
| AC 自动机 | O(M × σ) (Trie 节点×字母表)  |

对于本题：M = 2+3+3+4 = 12, σ = 26, 节点 ≤ 30 → 空间 ~30×26×4 ≈ 3KB，非常小。

### 5.3 实际场景分析

```
场景: 在 1MB 文本中搜索 100 个模式 (每个平均 10 字符)

暴力:  100 × 1M × 10 ≈ 10^9 次比较 → 数秒
KMP:   100 × 1M ≈ 10^8 次比较 → 约1秒
AC:    1M + 1000×26 ≈ 10^6 次操作 → 毫秒级

结论: 当模式数量多时，AC自动机的优势极其显著。
```

---

## 6. 常见错误

| 错误                             | 后果                                          | 正确做法                              |
| -------------------------------- | --------------------------------------------- | ------------------------------------- |
| `children[]` 用 0 表示"无子节点" | 混淆根节点 (0) 与"无子节点"，导致错误跳转     | 用 -1 表示"无子节点"                  |
| 失败链接用 DFS 而非 BFS          | 深层节点可能引用未计算的 fail                 | 必须用 BFS 队列层序遍历               |
| 输出合并时忘记越界检查           | `output_count` 超过 `MAX_PATTERNS` → 数组越界 | 检查 `output_count < MAX_PATTERNS`    |
| `search()` 中匹配失败直接回根    | 漏掉中间状态的匹配（如 "he" 在 "she" 内部）   | 沿 fail 链逐步回退，不能直接跳根      |
| 匹配位置计算错误                 | 输出错误的起始位置                            | `start = i - strlen(pattern) + 1`     |
| 忘记合并 fail 链上的输出         | 漏报匹配（如本题漏掉 "he"）                   | `bfs_build_fail()` 中显式复制 outputs |
| `fail` 初始化为 -1               | BFS 时访问 `nodes[-1]` → 段错误               | `fail` 初始化为 0（根节点）           |
| 忘记 `#include <string.h>`       | `strcpy`/`strlen` 隐式声明警告                | 确保头文件齐全                        |
| 循环中 `i` 类型为 `unsigned`     | 与 `strlen` 比较时可能下溢                    | 用 `int` 或 `size_t` 并注意比较       |

---

## 7. 重要知识点

1. **失败链接 = KMP 前缀函数的 Trie 推广**: KMP 在一维模式串上定义前缀函数，AC 自动机在 Trie（二维）上定义失败链接。本质相同：匹配失败时跳转到「当前已匹配前缀的最长真后缀」。

2. **BFS 的必要性**: 失败链接存在层级依赖——子节点的 fail 依赖于父节点的 fail。BFS 保证处理子节点前父节点的 fail 已就绪。

3. **输出合并机制**: 这是 AC 自动机「一次扫描找到所有匹配」的关键。不合并输出会漏掉嵌套/重叠的模式（如本题 "he" 嵌套在 "she" 中）。

4. **Trie 节点的双重身份**: 每个 Trie 节点同时是自动机的「状态」——表示「当前已匹配的前缀」。状态转移由 children 和 fail 共同决定。

5. **字母表大小影响**: 本题使用固定数组 children[26]，查找 O(1)。若字母表很大（如 Unicode），可改用哈希表以节省空间。

6. **AC 自动机是确定性有限自动机 (DFA)**: 加上失败链接后，每个状态对每个输入字符都有确定的下一个状态（要么 children 匹配成功，要么沿 fail 链找到匹配，要么回到根）。

---

## 8. 课堂讨论

### 8.1 如果把文本换成 "hishers"，会匹配到什么？

文本 "hishers": h i s h e r s

```
Step 0: 'h' → Node 1
Step 1: 'i' → Node 6
Step 2: 's' → Node 7, 匹配 "his"@0
Step 3: 'h' → 从7沿fail[7]=3, 3有'h'→4, cur=4
Step 4: 'e' → Node 5, 匹配 "she"@2, "he"@3
Step 5: 'r' → 从5沿fail[5]=2, 2有'r'→8, cur=8
Step 6: 's' → Node 9, 匹配 "hers"@3

最终: "his"@0, "she"@2, "he"@3, "hers"@3
```

AC 自动机在一次扫描中找到了 4 个匹配，包括重叠和嵌套的模式。

### 8.2 AC 自动机与正则表达式引擎的关系？

AC 自动机可以看作正则表达式 `(P₁|P₂|...|Pₖ)` 的确定性匹配引擎。Unix 的 `fgrep` (固定字符串 grep) 就是用 AC 自动机实现的。更复杂的正则引擎（如 `egrep`）使用 Thompson NFA 或回溯。

### 8.3 为什么不用「把所有模式用 | 连起来做 KMP」？

多个模式拼接后的前缀函数无法区分不同模式的边界。例如 "he|she" 拼接后，KMP 的前缀函数会把 "he" 和 "she" 的字符混在一起，无法独立报告每个模式的匹配位置。

### 8.4 AC 自动机的实际应用有哪些？

- **入侵检测系统 (IDS)**: Snort 使用 AC 自动机匹配网络包中的攻击特征
- **反病毒软件**: ClamAV 使用 AC 自动机匹配病毒签名
- **搜索引擎**: 关键词高亮和敏感词过滤
- **DNA 序列分析**: 在基因组中搜索多个 motif
- **输入法**: 拼音到汉字的 Trie 匹配

### 8.5 如果模式数量极大（百万级），AC 自动机会遇到什么问题？

内存成为瓶颈。一个百万模式的 AC 自动机可能需要数 GB 内存（每个节点 26×4=104 字节）。解决方案：

- **双数组 Trie (Double-Array Trie)**: 压缩存储，内存减少 10-100 倍
- **压缩 AC 自动机**: 合并单分支路径（类似压缩 Trie）
- **分块策略**: 将模式分到多个小的 AC 自动机，并行匹配

### 8.6 本题用数组而非指针实现 Trie 的优缺点？

| 维度       | 数组实现（本题）               | 指针实现                  |
| ---------- | ------------------------------ | ------------------------- |
| 查找速度   | O(1) 直接索引                  | O(1) 指针解引用           |
| 内存       | 每个节点 26×4=104B（可能浪费） | 每个子节点 8B（按需分配） |
| 代码复杂度 | 低                             | 中（需 malloc/free）      |
| 适用场景   | 字母表小（a-z）                | 字母表大或稀疏            |

对于字母表 a-z 且节点 ≤ 30，数组实现最简洁。

---

## 9. 与已有课程的联系

| 课程                              | 关联知识点                                  |
| --------------------------------- | ------------------------------------------- |
| Lesson 25 `my_strstr`             | 暴力单模式匹配 — AC 自动机的基础对比        |
| Lesson 40 `binary_search`         | 搜索空间缩小 — 与 fail 链的「跳转」思想类似 |
| Lesson 36 `binary_tree_traversal` | BFS 层序遍历 — 与 fail 构建的 BFS 一致      |
| Lesson 51 `ll1-table-parser`      | 确定性有限自动机 — AC 是 DFA 的一种         |
| Lesson 35 `queue_base`            | 队列数据结构 — BFS 构建 fail 的基础         |

---

## 10. 后续衔接

- **后续课程**: 正则表达式引擎（Thompson NFA 构造）、编译器词法分析（DFA 最小化）、后缀自动机 (SAM)
- **实际应用**: `fgrep -f patterns.txt text.txt` 的内部实现、Snort/ClamAV 的模式匹配引擎、Elasticsearch 的 percolator 查询
- **进阶算法**: Double-Array Trie、Aho-Corasick 的流式变体（在线匹配）、Wu-Manber 算法（另一种多模式匹配方法）

---

## 11. 参考资料

1. Aho, A. V., & Corasick, M. J. (1975). "Efficient String Matching: An Aid to Bibliographic Search". _Communications of the ACM_, 18(6), 333–340.
2. Crochemore, M., & Rytter, W. (1994). _Text Algorithms_. Oxford University Press. Chapter 3: String Matching with Automata.
3. Gusfield, D. (1997). _Algorithms on Strings, Trees, and Sequences_. Cambridge University Press. Chapter 3.4: The Aho-Corasick Algorithm.
4. `man 3 strcpy`, `man 3 strlen` — C 标准库字符串函数
5. Snort IDS 源码：`src/dynamic-preprocessors/sdf/acsmx.c` — 工业级 AC 实现参考
