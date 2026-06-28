# Unit 3 重构实施方案：C Panorama — 计算机科学全景图

> **状态**: ✅ 已评审通过  
> **日期**: 2026-06-28  
> **目标**: 将 Unit 3 (lesson 49-72) 从"系统软件工具链纵深"重构为"CS 17 子领域广度覆盖"  
> **前置依赖**: 无（纯替换原 Unit 3 内容，不影响其他 Unit）

---

## 一、背景与动机

当前 Unit 3（C Classicals）的 24 道练习高度聚焦于"系统软件与编程语言工具链"：

- 49-54: 文本处理与数据压缩（FSM/Regex/RLE/LZ77/Base64/JSON）
- 55-60: 系统编程（内存分配器/协程/TCP/HTTP/信号/shell）
- 61-66: 底层汇编与链接（RISC-V 汇编器/编码器/反汇编/ELF/工具链项目）
- 67-72: 迷你编译器前端（tokenizer/表达式解析/AST/栈 VM/树解释器/迷你语言）

这个纵深线路本质上是一个"迷你 Unit 4"，与 Unit 4（C Compiler）形成严重的功能重叠。作为 OpenCamp C 训练营的第三站，Unit 3 应该承担"CS 广度采样"的角色——让学生通过 24 道经典练习触摸 17+ 个 CS 子领域。

## 二、设计原则

1. **零依赖**: 所有练习仅使用 C 标准库（C11），绝无第三方库
2. **可自动化测试**: 每个练习支持 `clings check unit3 --solutions` 自动判分
3. **第一性原理**: 每道题瞄准该 CS 领域的"最简化可行版本"
4. **渐进难度**: 49→72 从基础到前沿，保持自然梯度
5. **不重复现有内容**: 严格避免与 Unit 0/1/2 已有练习重叠
6. **符合 CS2023**: 参考 ACM/IEEE/AAAI 联合发布的最新 CS 课程指南

## 三、24 题完整方案

### 分组 1: 系统与基础 (49-55)

| Lesson | 题名                     | 文件夹名                   | CS 领域 | 模式   | 核心知识点                                |
| ------ | ------------------------ | -------------------------- | ------- | ------ | ----------------------------------------- |
| 49     | 哲学家就餐问题           | `dining-philosophers-sync` | OS      | stdout | 死锁、资源竞争、同步、状态机模拟          |
| 50     | 可靠数据传输（停等协议） | `rdt-stop-and-wait`        | NC      | stdout | 超时重传、ACK、序号空间、丢包模拟         |
| 51     | 表驱动 LL(1) 解析器      | `ll1-table-parser`         | FPL     | stdout | FIRST/FOLLOW 集、预测分析表、自顶向下解析 |
| 52     | B+ 树索引实现            | `bplus-tree-index`         | DM      | stdout | 多路平衡树、叶子链表、范围查询            |
| 53     | 基础光线追踪渲染器       | `raytracer-from-scratch`   | GIT     | stdout | 光线投射、球体求交、Phong 光照、PPM       |
| 54     | A*寻路算法               | `astar-pathfinding`        | AI      | stdout | 可采纳启发式、Open/Closed 集、路径重建    |
| 55     | RSA 公钥加密演示         | `rsa-crypto-demo`          | SEC     | stdout | 模幂、欧拉函数、密钥生成、uint64 玩具版   |

### 分组 2: 分布式与底层 (56-60)

| Lesson | 题名                    | 文件夹名                    | CS 领域 | 模式   | 核心知识点                          |
| ------ | ----------------------- | --------------------------- | ------- | ------ | ----------------------------------- |
| 56     | 向量时钟实现            | `vector-clocks-distributed` | PDC     | stdout | Happens-Before、因果关系、偏序关系  |
| 57     | 缓存模拟器（LRU）       | `cache-simulator-lru`       | AR      | stdout | 组相联、局部性、命中率统计          |
| 58     | Aho-Corasick 多模式匹配 | `aho-corasick`              | AL      | stdout | Trie + 失败链接、一次扫描匹配多模式 |
| 59     | NFA 模拟与子集构造      | `nfa-subset-construction`   | AL      | stdout | ε-闭包、幂集构造、NFA→DFA           |
| 60     | 微型单元测试框架        | `micro-test-framework`      | SDF     | stdout | 宏断言、测试统计、TDD 基础          |

### 分组 3: 安全与计算 (61-65)

| Lesson | 题名                  | 文件夹名                  | CS 领域 | 模式   | 核心知识点                          |
| ------ | --------------------- | ------------------------- | ------- | ------ | ----------------------------------- |
| 61     | 缓冲区溢出分析        | `buffer-overflow-lab`     | SEC     | stdout | 栈布局、金丝雀、ASLR、安全分析      |
| 62     | 感知机分类器          | `perceptron-classifier`   | AI      | stdout | 线性分类、误分类驱动、收敛定理      |
| 63     | 矩阵 LU 分解求解器    | `lu-decomposition-solver` | MSF     | stdout | 部分主元、前向/后向替换、数值稳定性 |
| 64     | 无锁环形缓冲区        | `lockfree-ringbuffer`     | SF      | stdout | C11 _Atomic、SPSC、内存顺序         |
| 65     | 标记 - 清除垃圾回收器 | `mark-sweep-gc`           | FPL     | stdout | 根集合、对象图遍历、可达性分析      |

### 分组 4: 理论与前沿 (66-72)

| Lesson | 题名                  | 文件夹名               | CS 领域 | 模式   | 核心知识点                        |
| ------ | --------------------- | ---------------------- | ------- | ------ | --------------------------------- |
| 66     | 图灵机模拟器          | `turing-machine-sim`   | AL      | stdout | 状态转移、读写头、aⁿbⁿ 识别       |
| 67     | 快速傅里叶变换        | `cooley-tukey-fft`     | MSF     | stdout | 分治、蝴蝶操作、位反转、频谱分析  |
| 68     | Sobel 边缘检测        | `sobel-edge-detection` | CV/GIT  | stdout | 离散微分、梯度幅值、PGM 图像      |
| 69     | TF-IDF 文档相似度     | `tf-idf-cosine-sim`    | IR      | stdout | 词频、逆文档频率、余弦相似度      |
| 70     | 量子比特与门电路模拟  | `qubit-gate-simulator` | QC      | stdout | Pauli 门、Hadamard、布洛赫球      |
| 71     | 简化工作量证明（PoW） | `simple-proof-of-work` | BC      | stdout | SHA-256、难度目标、区块链         |
| 72     | ANSI 终端计算器       | `ansi-terminal-calc`   | HCI     | stdout | 终端控制序列、屏幕缓冲、交互式 UI |

## 四、CS2023 知识领域覆盖矩阵

| CS2023 知识领域                                | 覆盖题号   | 验证状态 |
| ---------------------------------------------- | ---------- | -------- |
| **AL** Algorithmic Foundations                 | 58, 59, 66 | —        |
| **AR** Architecture & Organization             | 57         | —        |
| **AI** Artificial Intelligence                 | 54, 62     | —        |
| **DM** Data Management                         | 52         | —        |
| **FPL** Foundations of Programming Languages   | 51, 65     | —        |
| **GIT** Graphics & Interactive Techniques      | 53, 68     | —        |
| **HCI** Human-Computer Interaction             | 72         | —        |
| **MSF** Mathematical & Statistical Foundations | 63, 67     | —        |
| **NC** Networking & Communication              | 50         | —        |
| **OS** Operating Systems                       | 49         | —        |
| **PDC** Parallel & Distributed Computing       | 56         | —        |
| **SEC** Security                               | 55, 61     | —        |
| **SF** Systems Fundamentals                    | 64         | —        |
| **SDF** Software Development Fundamentals      | 60         | —        |
| **新兴领域**                                   | 69, 70, 71 | —        |

**覆盖：14/17 正式领域 + 3 新兴 = 17 领域**

## 五、关键设计决策与风险缓解

### 5.1 哲学家就餐 — "免 pthread"方案

**决策**: 在单线程中用状态机模拟 5 个哲学家的并发行为，避免 pthread 依赖。

```
main loop:
  while (eat_count < target):
    pick a random philosopher
    if thinking → try to grab chopsticks (check neighbors)
    if eating → after N rounds, release chopsticks
    print state snapshot
```

输出格式：每个哲学家有 thinking/hungry/eating 三种状态，逐轮输出状态变化。

### 5.2 RSA — "uint64 玩具版"

**决策**: 限制 p, q < 2^16（即 n < 2^32），所有运算在 uint64_t 范围内完成。

- 快速幂取模：`mod_pow(base, exp, mod)`
- 扩展欧几里得：求私钥 d
- 输入：p, q, plaintext → 输出：n, e, d, ciphertext, decrypted
- 测试：给定固定的 p, q, message，验证加密再解密后一致

### 5.3 缓冲区溢出 — "安全分析"替代方案

**决策**: 不做实际溢出利用（平台/编译器版本极度敏感），改为分析型练习。

```
输入: 一段 C 代码（如带 gets() 的程序）
任务:
  1. 画出该函数的栈帧布局（ASCII 图）
  2. 计算变量到返回地址的偏移量
  3. 判断是否存在溢出风险
  4. 解释金丝雀（canary）的防护原理
输出: 结构化的安全分析报告（固定格式，便于自动对比）
```

测试：用固定模板 + 关键词匹配验证分析正确性。

### 5.4 LL(1) 表驱动解析器

**决策**: 替换原方案的"递归下降表达式解析器"（与现有 68 重叠）。学生需：

1. 读取解析表（硬编码在代码中）
2. 对输入表达式进行自顶向下的表驱动分析
3. 输出推导过程序列

```
给定文法: E → TE', E' → +TE' | ε, T → FT', T' → *FT' | ε, F → (E) | id
输入: id + id * id
输出: 每一步的栈内容和输入缓冲区
```

### 5.5 Aho-Corasick 多模式匹配

**决策**: 替换八皇后（与 Unit 2 29 重复）。构建 Trie + 失败链接，实现：

1. 构建模式集合的 Trie
2. BFS 构建失败链接
3. 扫描文本输出所有匹配位置

```
输入: patterns = {"he", "she", "his", "hers"}, text = "ushers"
输出: 每个匹配的模式、起始位置
```

### 5.6 微型测试框架

**决策**: 替换内存池分配器（与 55 重叠），覆盖 SDF 领域。用宏实现：

```
ASSERT_EQ(a, b)    — 比较整数值
ASSERT_STREQ(a, b) — 比较字符串
TEST(name) { ... }  — 定义测试用例
RUN_TESTS()         — 运行所有测试，汇总通过/失败
```

测试：框架自带自测——用该框架测试框架自身。

### 5.7 TF-IDF 文档相似度

**决策**: 替换 LCS（Unit 2 已覆盖），引入信息检索基础。

```
输入: 3-5 篇短文档（内嵌在代码中）
任务: 计算 TF-IDF 向量，输出两两之间的余弦相似度
输出: 相似度矩阵
```

### 5.8 ANSI 终端计算器（替代终端编辑器）

**决策**: 终端编辑器（vi 简化版）过度依赖平台终端能力，改为终端计算器。

- 使用 ANSI escape sequences 做光标定位和彩色输出
- 支持四则运算和括号
- 实时显示输入/输出（类似 bc 但带终端美化）
- 覆盖 HCI 的"人与计算机交互"核心：输入→处理→反馈循环

## 六、文件结构模板

每个练习目录遵循已确立的标准结构：

```
exercises/<folder-name>/
├── <source>.c          # 学生编辑的源文件（含 #error TODO:）
├── README.md           # 详细文档（参考 29_eight_queens 标准）
├── exercises.toml      # 练习元数据
└── Makefile            # 可选：编译与本地测试
```

### 6.1 exercises.toml 模板

```toml
[[exercises]]
name = "<lesson>_<folder-name>"
title = "<中文标题> <English Title>"
unit = "unit3"
lesson = <lesson>
order = <order>
mode = "stdout"
source = "<source>.c"
lesson_url = "https://cnb.cool/q.qq/opencamp-c-2026-summer/Unit-3-C-Panorama/-/tree/main/lessons/<lesson>_<folder-name>"
hint = '''<简明提示>'''

[[exercises.cases]]
stdin = "<可选：标准输入>"
stdout = "<期望的标准输出>"
```

### 6.2 README.md 质量标准

参考 `29_eight_queens/README.md`（277 行），每个 README 必须包含：

1. **课程任务** — 明确学生要完成什么
2. **前置知识** — 该 CS 领域的必要背景
3. **ASCII 图解** — 数据结构/算法流程的可视化
4. **算法详解** — 逐步推导过程
5. **量化对比** — 不同方法的复杂度/效率对比表
6. **常见错误** — 陷阱与解决
7. **课堂讨论** — 深度思考题
8. **参考资料** — Wikipedia、经典教材、论文

### 6.3 源文件模板

```c
/* <lesson>_<name>.c — <title>
 *
 * 任务：1. <子任务 1>
 *       2. <子任务 2>
 *       <期望输出>
 *
 * 背景：<该 CS 领域的简要背景>
 *
 * 算法：<核心思路>
 *
 * 知识点：<关键字列表>
 *
 * 验证：<输入> → <期望输出>
 */
#include <stdio.h>
#include <stdlib.h>

/* 可提供部分骨架代码 */

int main(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    return 0;
}
```

## 七、实施阶段

### Phase 0: 清理旧 Unit 3（1 步）

- [ ] 将 `exercises/49_*` 到 `exercises/72_*` 的所有旧目录归档（移到 `archive/unit3-old/` 或直接删除）

### Phase 1: 分组 1 — 系统与基础（49-55）

- [ ] 49_dining-philosophers-sync — 哲学家就餐（状态机模拟版）
- [ ] 50_rdt-stop-and-wait — 停等协议
- [ ] 51_ll1-table-parser — LL(1) 表驱动解析器
- [ ] 52_bplus-tree-index — B+ 树索引
- [ ] 53_raytracer-from-scratch — 光线追踪
- [ ] 54_astar-pathfinding — A*寻路
- [ ] 55_rsa-crypto-demo — RSA 加密

### Phase 2: 分组 2 — 分布式与底层（56-60）

- [ ] 56_vector-clocks-distributed — 向量时钟
- [ ] 57_cache-simulator-lru — 缓存模拟器
- [ ] 58_aho-corasick — Aho-Corasick 多模式匹配
- [ ] 59_nfa-subset-construction — NFA 子集构造
- [ ] 60_micro-test-framework — 微型测试框架

### Phase 3: 分组 3 — 安全与计算（61-65）

- [ ] 61_buffer-overflow-lab — 缓冲区溢出分析
- [ ] 62_perceptron-classifier — 感知机分类器
- [ ] 63_lu-decomposition-solver — LU 分解
- [ ] 64_lockfree-ringbuffer — 无锁环形缓冲区
- [ ] 65_mark-sweep-gc — 标记 - 清除 GC

### Phase 4: 分组 4 — 理论与前沿（66-72）

- [ ] 66_turing-machine-sim — 图灵机模拟器
- [ ] 67_cooley-tukey-fft — FFT
- [ ] 68_sobel-edge-detection — Sobel 边缘检测
- [ ] 69_tf-idf-cosine-sim — TF-IDF 文档相似度
- [ ] 70_qubit-gate-simulator — 量子比特模拟
- [ ] 71_simple-proof-of-work — 简化 PoW
- [ ] 72_ansi-terminal-calc — ANSI 终端计算器

### Phase 5: 集成验证

- [ ] 更新 `clings.toml` — Unit 3 title 改为 "C Panorama"
- [ ] 更新 `CURRICULUM.md` — 修改 Unit 3 描述
- [ ] 运行 `python -m clings check unit3 --solutions` 验证全部通过
- [ ] 编写 solutions/ 目录下的参考答案

## 八、技术约束与规范

### 8.1 编译标准

- 编译器：gcc
- 标准：C11 (`-std=c11`)
- 警告：`-Wall -Wextra`
- 禁止使用的头文件：无第三方库，仅 C 标准库

### 8.2 测试模式

- 主要使用 `mode = "stdout"` — 比对程序标准输出
- 复杂输出用精确字符串匹配：
  ```toml
  [[exercises.cases]]
  stdout = """expected output line 1
  expected output line 2"""
  ```
- 极少数情况使用 `compile_only = true`（如 60 微型测试框架的自测）

### 8.3 Order 编号

- 49 → order=74, 50→75, ... 72→97（紧接 Unit 2 最后一题 29_eight_queens 的 order=77 之后...）
- 需要重新规划所有 order。实际 order 从 Unit 0 到 Unit 4 全局连续编号。

### 8.4 命名规范

- 文件夹名：`{lesson}_{kebab-case-name}`
- 源文件名：`{kebab-case-name}.c` 或 `{lesson}_{name}.c`
- 在 exercises.toml 中 `name = "{lesson}_{kebab-case-name}"`

## 九、难度评估

| 难度          | 数量 | 题目                                           |
| ------------- | ---- | ---------------------------------------------- |
| ⭐⭐ 中等     | 5    | 49, 50, 54, 60, 62                             |
| ⭐⭐⭐ 较难   | 12   | 51, 52, 55, 56, 57, 58, 59, 63, 66, 68, 69, 72 |
| ⭐⭐⭐⭐ 困难 | 7    | 53, 61, 64, 65, 67, 70, 71                     |

难度分布合理：中等 21%、较难 50%、困难 29%。Unit 3 作为"采样拼盘"，核心是让学生体验每个领域的思想，难度不必均匀——有些领域天然比另一些复杂。

## 十、验收标准

1. **完整性**: 24 个练习目录全部创建，每个含 `.c`、`README.md`、`exercises.toml`
2. **正确性**: 所有 solutions 通过 `clings check unit3 --solutions` 测试
3. **文档质量**: 每个 README.md >= 150 行，包含 ASCII 图解和量化对比
4. **独立性**: 各练习之间无依赖，可独立完成
5. **零依赖**: 不引入任何第三方 C 库
6. **CS2023 对齐**: 覆盖 14 个正式知识领域 + 3 个新兴领域

---

> **下一步**: 按 Phase 1→2→3→4→5 顺序实施。每个 Phase 完成时运行 `python -m clings check unit3 --solutions` 做局部验证。
