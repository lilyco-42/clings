# Clings

[![PyPI][badge-pypi]][pypi]
[![Python][badge-python]][pypi]
[![Downloads][badge-downloads]][pypi]
[![License][badge-license]][pypi]

> **C 语言版 [Rustlings](https://github.com/rust-lang/rustlings)** — 通过修复编译错误、补全代码来学习 C 语言。
>
> 🏕️ **[Learning C Camp 2026](https://opencamp.cn/C/camp/2026)** 练习工具 &nbsp;|&nbsp; 📅 2026.06.28 – 07.26 &nbsp;|&nbsp; 4 个 Unit · 72 课 · 约 97 道练习

---

## 目录

- [概述](#概述)
- [快速开始](#快速开始)
- [安装方式](#安装方式)
- [课程路线图](#课程路线图)
- [核心特性](#核心特性)
- [命令参考](#命令参考)
- [练习系统](#练习系统)
- [项目架构](#项目架构)
- [开发指南](#开发指南)
- [CI/CD](#cicd)
- [常见问题](#常见问题)
- [相关资源](#相关资源)
- [Contributors](#contributors)
- [License](#license)

---

## 概述

**Clings** 是一个 Rustlings 风格的 C 语言交互式练习 CLI 工具，专为 OpenCamp Learning C Camp 2026 训练营打造。它将 Rust 社区久经考验的 **"compiler-driven teaching"**（编译器驱动教学）范式引入 C 语言学习：

```
┌──────────┐     ┌──────────┐     ┌──────────┐     ┌──────────┐
│  读报错   │ ──→ │  修代码   │ ──→ │   保存    │ ──→ │  自动验证  │
└──────────┘     └──────────┘     └──────────┘     └──────────┘
      ↑                                                  │
      └──────────────────────────────────────────────────┘
                      循环迭代直到通过
```

### 适用人群

- **零基础学员**：从 `int main(void) { return 0; }` 开始，一步步建立编程心智模型
- **有基础开发者**：通过实现数据结构、经典算法、系统编程来巩固 C 语言功底
- **计算机专业学生**：覆盖 CS 17 个子领域的经典问题，建立全局视野
- **进阶学员**：为编译器项目（Unit 4）和内核项目（Unit 5）建立坚实基础

### 与 Rustlings 的对比

| 维度 | Rustlings | Clings |
|------|-----------|--------|
| 语言 | Rust | C |
| 安装方式 | `cargo install rustlings` | `uvx clings` / `pipx install clings` |
| 运行时 | Rust 工具链 | Python 3.11+ + GCC |
| 依赖 | Cargo 生态 | **零第三方依赖**（纯 Python stdlib） |
| 练习规模 | ~94 题 | **约 97 题**（4 个 Unit） |
| 评测模式 | 编译 + 测试 | return / stdout / compile / make+stdout |
| 配套课程 | Rust Book | OpenCamp C 2026 Summer 训练营 |

---

## 快速开始

```bash
# 推荐：一行命令启动（无需预先安装 clings）
uvx clings init unit0    # 初始化 Unit 0 练习到当前目录
# → created clings.toml
# → ✅ Initialized 5 lesson directories (9 exercises) for unit0
# → copied 21 files, skipped 0 existing .c files
# → Run clings to start!

uvx clings               # 进入交互式 watch 模式，开始做题
```

> **环境要求**：Python 3.11+ &nbsp;|&nbsp; GCC（或 Clang）&nbsp;|&nbsp; Linux / macOS / WSL2

初始化后目录结构：

```
my-clings/
├── 01_simplest_c_program/    # ← 在这里修改 .c 文件
├── 02_hello_world_printf/
├── 03_loop_counting/
├── ...                        # 共 9 道 Unit 0 练习题
├── clings.toml                # Unit 配置
└── .clings-state.txt          # 自动生成的进度文件（完成练习后）
```

> 也可以一次初始化全部 Unit：`uvx clings init all`（72 个目录，约 97 道练习，覆盖 Unit 0-3）

---

## 安装方式

### 方式一：uvx（推荐，零安装）

```bash
curl -LsSf https://astral.sh/uv/install.sh | sh   # 安装 uv
uvx clings init unit0                               # 直接运行，无需 pip install
uvx clings                                          # 进入 watch 模式
```

`uvx` 自动下载隔离的临时环境运行 clings，不会污染系统 Python。

### 方式二：pipx（全局安装，隔离环境）

```bash
pipx install clings
clings init unit0
clings
```

### 方式三：pip + venv（传统虚拟环境）

```bash
python3 -m venv .venv && source .venv/bin/activate
pip install clings
clings init unit0
clings
```

> ⚠️ **注意**：Ubuntu 23.04+ / Debian 12+ 默认启用 PEP 668，禁止直接 `pip install` 到系统 Python。请使用上述 uvx / pipx / venv 三种方式之一，避免 `--break-system-packages`。

### 指定版本

```bash
uvx clings@4.5.0 init unit0    # 锁定版本（推荐教学环境统一版本）
uvx clings@latest init unit0   # 始终使用最新版
```

---

## 课程路线图

Learning C Camp 2026 训练营共 6 个 Unit。其中 **Unit 0-3（4 个阶段）通过 clings 完成**，Unit 4-5 为独立项目阶段（需要 QEMU + 交叉编译工具链，详见训练营对应仓库）：

| Unit | 名称 | 阶段 | Lessons | 题数 | 直播时间 | 核心内容 |
|:----:|------|:----:|:-------:|:----:|----------|----------|
| 0 | C Primer | 导学 | 01–05 | 9 | —（纯自学） | 最简程序、printf、循环、条件分支、累加求和 |
| 1 | C Fundamentals | 基础 | 06–24 | 40 | 06/29 + 07/01 | 嵌套循环、函数、数组、指针、结构体、联合体、位运算、状态机、词法分析器 |
| 2 | C Essentials | 进阶 | 25–48 | 24 | 07/03 + 06 + 08 + 10 | 链表、栈、队列、树、堆、排序、查找、回溯、C 标准库深入 |
| 3 | C Classicals | 专业 | 49–72 | 24 | 07/13 + 15 + 17 | 17 个 CS 子领域经典问题（OS/网络/编译/AI/密码学/量子计算…） |
| 4 | C Compiler | 项目 | 73–96 | — | 待定 | 从零实现 C 编译器 nccl-cc（独立仓库，需 QEMU） |
| 5 | C Kernel | 项目 | — | — | 待定 | 操作系统内核 Avatar OS（独立仓库，需 QEMU） |

```
   clings 覆盖                          独立项目（需 QEMU + 交叉编译工具链）
┌──────────────────────────────┐    ┌──────────────────────────────┐
│ Unit 0 ──→ Unit 1 ──→       │    │                              │
│             Unit 2 ──→ Unit 3│    │  Unit 4 (编译器) + Unit 5 (内核) │
│ (入门 → 基础 → 进阶 → 专业)  │    │  (独立仓库, 独立工具链)        │
└──────────────────────────────┘    └──────────────────────────────┘
   "语言 → 算法 → 系统" 知识闭环       "编译器 → 内核" 系统级实践
```

### 各 Unit 知识点速览

<details>
<summary><b>Unit 0 · C Primer（展开）</b></summary>

5 个最小可运行程序，建立"编辑 → 编译 → 运行"全流程直观认知：

| Lesson | 题目 | 知识点 |
|--------|------|--------|
| 1 | 最简单的 C 程序 | `main` 函数、`return`、变量与表达式 |
| 2 | Hello World | `printf`、`#include`、格式化输出 `%d %x %p` |
| 3 | 循环打印 | `while` / `for` 循环、自增 `++`、关系运算符 |
| 4 | 判断奇偶 | `scanf`、`if-else`、取模 `%`、取地址 `&` |
| 5 | 1 到 100 求和 | 累加模式、`continue`、`+=`、条件编译 `#ifdef` |

</details>

<details>
<summary><b>Unit 1 · C Fundamentals（展开）</b></summary>

19 个递进式 Lesson，从九九乘法表到搜索引擎，覆盖 C 语法全貌：

| Lesson | 题目 | 知识点 |
|--------|------|--------|
| 6 | 九九乘法表 | 嵌套 `for` 循环 |
| 7 | 100 以内最大素数 | 函数定义与调用、`break` |
| 8 | 统计数字 9 的个数 | 函数封装、逐位提取 |
| 9 | 整数转字符串 | 字符数组、`'\0'` 结尾、`itoa` |
| 10 | 约瑟夫环 | 数组模拟链表 |
| 11 | 两点距离 | `struct` 结构体、`sqrt` |
| 12 | 判断大小端 | `union` 联合体、内存布局 |
| 13 | 车辆限行 | `switch-case`、三元运算符 `?:` |
| 14 | 迷宫出路 | 二维数组、DFS 思想 |
| 15 | 统计 bit 1 个数 | 位运算 `& | ^ ~ << >>`、popcount |
| 16 | 实现 strcpy | 指针解引用 `*`、指针递增 |
| 17 | 单词计数 | 状态机 FSM 思想 |
| 18 | 实现 printf | 可变参数 `va_list`、格式化解析 |
| 19 | 命令解释器 Shell | 函数指针、命令分发 |
| 20 | 预处理器 | 状态机去注释、`#define` 宏展开 |
| 21 | 词法分析器 | DFA 识别关键字/标识符/数字/运算符 |
| 22 | 猜数字游戏 | `rand()`、双重循环 ?A?B |
| 23 | 五子棋 | 二维数组棋盘、方向扫描判胜 |
| 24 | 搜索引擎 | 字符串匹配、HTML 链接提取 |

</details>

<details>
<summary><b>Unit 2 · C Essentials（展开）</b></summary>

24 个 Lesson，从零实现经典数据结构 + 深入 C 标准库：

**数据结构**：单向链表（插入/删除/反转/环检测）、循环链表（约瑟夫环）、栈（括号匹配）、环形队列、二叉树（DFS/BFS 遍历）、BST 操作、堆（Top-K）

**算法**：回溯（全排列/八皇后）、二分查找、快速排序

**C 标准库深入**：`memcpy` / `memmove` / `strstr` / `snprintf` / `strtok_r` / `qsort` / `realloc` / `sscanf` / `setjmp`-`longjmp` / I/O 缓冲性能

</details>

<details>
<summary><b>Unit 3 · C Classicals（展开）</b></summary>

24 道题横跨 CS 17 个子领域，每题都是该领域的"第一性原理"经典问题：

| 领域 | Lesson | 题目 |
|------|--------|------|
| 操作系统 | 49 | 哲学家就餐（死锁预防） |
| 网络 | 50 | 停等协议（可靠传输） |
| 编译原理 | 51 | 表驱动 LL(1) 解析器 |
| 数据库 | 52 | B+ 树索引 |
| 图形学 | 53 | 基础光线追踪 |
| AI | 54 | A* 寻路算法 |
| 密码学 | 55 | RSA 公钥加密 |
| 分布式 | 56 | 向量时钟 |
| 体系结构 | 57 | 缓存模拟器 LRU |
| 算法 | 58 | Aho-Corasick 多模式匹配 |
| 自动机 | 59 | NFA 模拟与子集构造 |
| 软件工程 | 60 | 微型单元测试框架 |
| 安全 | 61 | 缓冲区溢出分析 |
| 机器学习 | 62 | 感知机二分类器 |
| 数值计算 | 63 | 矩阵 LU 分解 |
| 嵌入式 | 64 | 无锁环形缓冲区 |
| PL 理论 | 65 | 标记-清除 GC |
| 可计算性 | 66 | 图灵机模拟器 |
| 信号处理 | 67 | Cooley-Tukey FFT |
| 计算机视觉 | 68 | Sobel 边缘检测 |
| 信息检索 | 69 | TF-IDF 文档相似度 |
| 量子计算 | 70 | 量子比特与门电路 |
| 区块链 | 71 | 简化工作量证明 PoW |
| HCI | 72 | ANSI 终端计算器 |

</details>

---

## 核心特性

### Watch 交互模式

```bash
clings                    # 进入 watch 模式（默认子命令）
clings watch unit1        # 从 Unit 1 开始
```

启动后进入全屏交互终端界面，每次修改 `.c` 文件并保存，自动触发编译 + 测试验证：

```
┌──────────────────────────────────────────────────────────┐
│  Clings Watch — Unit 0: C Primer                         │
│                                                          │
│  ● 01a_return_zero                 ✅ PASSED             │
│  ● 01b_return_expression           ✅ PASSED             │
│  ● 02a_hello_world                 🔧 COMPILING...       │
│  ○ 02b_format_print                ⬚ pending            │
│  ○ 03a_while_loop                  ⬚ pending            │
│  ...                                                     │
│                                                          │
│  Progress: 2/9 (22%)                                     │
│                                                          │
│  [h]int  [l]ist  [r]un  [t]ests  [q]uit                 │
└──────────────────────────────────────────────────────────┘
```

**Watch 模式快捷键**：

| 按键 | 功能 |
|:----:|------|
| `h` | 查看当前练习的提示 |
| `l` | 打开交互式练习列表（可跳转） |
| `t` | 显示当前练习的测试用例（TDD 开发） |
| `r` | 手动重新运行当前练习 |
| `n` | 切换到下一题（当前题通过后可用） |
| `c` | 检查所有练习，完成后自动跳到第一个未完成的题 |
| `x` | 重置当前练习文件到初始状态 |
| `q` | 退出 watch 模式 |

**Watch 模式选项**：

```bash
clings watch --manual-run     # 关闭文件变化自动重跑，按 r 手动触发
clings watch --auto-advance   # 当前题通过后自动切到下一题（默认关闭，需手动按 n）
clings watch --edit-cmd code  # 启动时自动用 VS Code 打开练习文件
```

### 多模式评测体系

Clings 支持四种评测模式，覆盖从入门到工程的完整学习曲线：

| 模式 | 说明 | 适用阶段 | 示例 |
|------|------|:--------:|------|
| `return` | 检查程序退出码 | Unit 0 入门 | `expected_return = 0` |
| `stdout` | 标准输出逐字符比对 | Unit 0–2 | 九九乘法表逐行匹配 |
| `compile` | 验证编译通过（无警告） | 语法练习 | 查看编译是否成功 |
| `make+stdout` | Makefile 编译 + stdout 比对 | Unit 3 专业阶段 | 工程化编译流程 |

评测在 `<exercise>.c` 同级目录的 `exercises.toml` 中定义，测试数据对于 `make+stdout` 模式存储在包内 `tests/` 目录中（学生无法修改，确保评测公正）。

### 内置提示系统

每道练习都配有精心设计的渐进式提示，引导学生思考而非直接给答案：

```bash
clings hint                    # 查看当前（下一道未完成）练习的提示
clings hint 06_multiplication  # 查看指定练习的提示
```

提示内容分两种风格：

- **Unit 0–2（入门到进阶）**：知识点引导 + 思考方向 + 参考资料。例如 Unit 0 的提示会引导你思考"需要什么返回类型、函数名、参数列表、函数体"
- **Unit 3（专业阶段）**：结构化提示，包含 Makefile 编写指南 + 分步骤 TODO 清单 + 概念解析（如哲学家就餐提示会列出 7 个 TODO：初始化、冲突检测、拿筷子、放筷子、判定完成、格式化输出、主循环三阶段）

### 进度追踪

```bash
clings list                    # 列出所有已初始化练习和进度
clings list unit1              # 只列出 Unit 1
clings list unit0 --check      # 重新编译验证后列出（较慢但准确）
```

输出格式（每行列：状态标记 + Unit + Lesson + 评测模式 + 练习名 + 中文标题）：

```
  • unit0 01 return   01a_return_zero                  最简单的C程序: 返回0
  • unit0 01 return   01b_return_expression            变量与表达式: global + local
  • unit0 02 stdout   02a_hello_printf                 printf: hello, world
  • unit0 02 stdout   02b_format_print                 格式化输出: %d 和 %x
  • unit0 03 stdout   03a_while_loop                   while 循环: 数到10
  • unit0 03 stdout   03b_for_loop                     for 循环: 从10倒数到1
  • unit0 04 stdout   04_odd_or_even                   if-else: 判断奇偶
  • unit0 05 stdout   05a_sum_1_to_100                 循环累加: 1到100求和
  • unit0 05 stdout   05b_sum_even                     continue: 只加偶数

  Progress: [░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░] 0/9 (0.0%)
```

进度条直观显示完成百分比，`.clings-state.txt` 记录每道题目的通过状态（纯文本，可加入 `.gitignore`）。

### CI 自动评分

```bash
clings score unit0               # 跑完全部题目，输出 CI 兼容的 set-output
clings score unit0 --json        # 同时在 stdout 输出 JSON 报告
```

运行过程输出（逐题进度）：

```
[1/9] 01a_return_zero PASSED (+1)
[2/9] 01b_return_expression PASSED (+1)
...
==================================================
  Total: 9  Passed: 9  Failed: 0
  Score: 9/9
  Report: clings_score.json
==================================================
##[set-output score=9]
```

`--json` 模式额外输出结构化报告：

```json
{
  "test_summary": {
    "total_exercises": 9,
    "passed_exercises": 9,
    "failed_exercises": 0,
    "total_score": 9
  },
  "exercises": [
    {
      "name": "01a_return_zero",
      "status": "PASSED",
      "score": 1
    },
    ...
  ]
}
```

`##[set-output score=N]` 格式可直接集成到 CNB / GitHub Actions 等 CI 系统，实现提交即打分。报告同时保存为 `clings_score.json` 文件。

### 零依赖原则

```
pip install clings   # 仅安装 Python 标准库，无任何第三方依赖
```

Clings 坚持**零第三方依赖**原则——所有功能仅使用 Python 标准库（`argparse`、`subprocess`、`tomllib`、`pathlib`、`os` 等）实现。这意味着：
- ✅ 任何 Python 3.11+ 环境即装即用
- ✅ 无需网络下载依赖包
- ✅ 无供应链安全风险
- ✅ 离线环境完全可用

---

## 命令参考

### 完整命令列表

| 命令 | 语法 | 说明 |
|------|------|------|
| `clings` | `clings [selector]` | **默认命令**：进入交互式 watch 模式 |
| `init` | `clings init [unit0\|unit1\|...\|all]` | 从包内释放练习文件到当前目录 |
| `watch` | `clings watch [selector]` | 交互式 watch（保存即编译验证） |
| `run` | `clings run [exercise]` | 运行单个练习（省略=下一道未完成，`random`=随机） |
| `hint` | `clings hint [exercise]` | 查看提示（省略=下一道未完成） |
| `tests` | `clings tests [exercise]` | 显示测试用例（TDD 开发参考） |
| `list` | `clings list [selector]` | 列出练习 + ✔/• 进度状态 |
| `check` | `clings check [selector]` | 批量验证（遇到失败立即退出） |
| `score` | `clings score [selector] [--json]` | CI 打分（跑完全部，输出 JSON 报告） |
| `reset` | `clings reset <exercise>` | 重置指定练习文件到初始状态 |
| `reset` | `clings reset progress` | 清除全部进度记录 |
| `doctor` | `clings doctor` | 检查开发环境（Python/GCC/路径） |
| `-v` | `clings -v` | 显示版本号 |

### Selector 语法

多数命令支持 `selector` 参数来筛选练习范围：

```bash
clings list unit0             # Unit 编号
clings list unit1             # Unit 编号
clings check 06               # Lesson 编号
clings run 06_multiplication  # 练习名（前缀匹配）
clings run random             # 随机一道题
clings watch unit2            # 限定 Unit 范围
```

### 常用场景示例

```bash
# 场景 1：初次使用
uvx clings init unit0         # 初始化 Unit 0
# → created clings.toml
# → ✅ Initialized 5 lesson directories (9 exercises) for unit0
# → copied 21 files, skipped 0 existing .c files
# → Run clings to start!
uvx clings                    # 开始交互式练习

# 场景 2：只看提示不运行
clings hint                   # 查看下一道未完成练习的提示
clings hint 12_little_endian  # 查看指定练习的提示

# 场景 3：TDD 开发（先看测试再编码）
clings tests 49_dining-philosophers-sync  # 查看期望输出
# → 显示完整测试用例（1 total, 1 runnable）
# → 展示 expected stdout（哲学家吃饭全程 14 轮状态追踪）
# → 编辑 .c 文件 + Makefile
clings run 49_dining-philosophers-sync    # 验证

# 场景 4：批量验证
clings check unit1 --solutions  # 用参考答案跑通全 Unit 1
# → [1/40] 06_multiplication_table PASSED
# → ...
# → 40/40 passed, 0 failed
clings score unit1 --json       # 输出评分 JSON

# 场景 5：重置
clings reset 06_multiplication  # 重置单个练习文件到初始状态
clings reset progress           # 清空所有进度

# 场景 6：环境诊断
clings doctor
# → python: 3.13.2
# → gcc: found (gcc 14.2.0)
# → make: found (GNU Make 4.3)
# → config: clings.toml
# → build dir: .clings/build
```

---

## 练习系统

### 练习目录结构

```
exercises/
├── 01_simplest_c_program/       # Lesson 1
│   ├── exercises.toml           # 练习元数据 + 测试用例
│   ├── 01a_return_zero.c        # 学生源码模板（含 #error TODO）
│   ├── 01b_return_expression.c  # 学生源码模板（含 #error TODO）
│   ├── README.md                # 课程讲义（BNF、扩展、讨论题）
│   ├── simple.s                 # 参考汇编
│   └── variable.c               # 参考示例
├── 02_hello_world_printf/       # Lesson 2
│   └── ...
└── ...
```

每道 `.c` 模板文件包含：
- 文件头注释（Lesson 信息、知识点、任务说明、验证方式）
- `#include` 等必要头文件（部分提供）
- `#error TODO: ...` 指令（阻止编译通过，提示学生需要完成的工作）

### 评测模式详解

```toml
# exercises/01_simplest_c_program/exercises.toml

[[exercises]]
name = "01a_return_zero"
title = "最简单的C程序: 返回0"
unit = "unit0"
lesson = 1
order = 1
mode = "return"               # ← 评测模式
expected_return = 0           # ← 期望退出码
source = "01a_return_zero.c"
hint = '''...'''               # ← 多行提示文本
```

```toml
# exercises/06_multiplication_table/exercises.toml

[[exercises]]
name = "06_multiplication_table"
mode = "stdout"               # ← stdout 比对模式

[[exercises.cases]]
stdin = ""
stdout = """
1*1=1
1*2=2	2*2=4
1*3=3	2*3=6	3*3=9
...
"""                            # ← 期望输出（逐字符比对）
```

### 公开测试数据

通过 `clings tests <exercise>` 查看测试用例，行为因评测模式而异：

- **Unit 0–2（return / stdout / compile 模式）**：测试用例内嵌在 `exercises.toml` 中，`clings tests` 显示 `(no public test cases available)`，因为答案已在模板文件中暗示
- **Unit 3（make+stdout 模式）**：测试用例独立存储在包内 `tests/` 目录中，学生无法修改，确保评测公正：

```
tests/
├── 49_dining-philosophers-sync.toml
├── 50_rdt-stop-and-wait.toml
├── 51_ll1-table-parser.toml
├── ...
└── 72_ansi-terminal-calc.toml
```

```bash
clings tests 49_dining-philosophers-sync
```

会输出完整结构化的测试用例：

```
=== 哲学家就餐问题 — Dining Philosophers ===
name: 49_dining-philosophers-sync
unit: unit3  lesson: 49
mode: make+stdout

Test Cases (1 total, 1 runnable):

[Case 1/1]
  expected stdout:
    === Dining Philosophers ===
    N=5, asymmetric (P4 right-first), target=3 meals each
    T=Thinking H=Hungry E=Eating
    ...
```

学员采用 TDD 开发模式——先读测试理解期望行为，再动手编码和编写 Makefile。

---

## 项目架构

### 仓库结构

```
clings/                         # 仓库根目录 (cli 分支)
├── clings/                     # Python 包（零依赖，纯 stdlib）
│   ├── __init__.py             # 包入口，导出 main
│   ├── __main__.py             # 支持 python -m clings
│   ├── py.typed                # PEP 561 标记
│   ├── cli.py                  # argparse 入口 + 命令分发
│   ├── config.py               # 配置、路径检测、练习发现、编译器检测
│   ├── compiler.py             # C 编译 + 测试执行引擎
│   ├── state.py                # 进度状态持久化 (.clings-state.txt)
│   ├── renderer.py             # Watch 模式终端 UI 渲染
│   ├── utils.py                # ANSI 颜色、终端工具、文件操作
│   └── commands/               # 子命令模块（10 个独立文件）
│       ├── check.py            # clings check — 批量验证
│       ├── doctor.py           # clings doctor — 环境诊断
│       ├── hint.py             # clings hint — 查看提示
│       ├── init.py             # clings init — 初始化练习
│       ├── list.py             # clings list — 列出进度
│       ├── reset.py            # clings reset — 重置练习/进度
│       ├── run.py              # clings run — 运行练习
│       ├── score.py            # clings score — CI 评分
│       ├── tests.py            # clings tests — 显示测试用例
│       └── watch.py            # clings watch — 交互式 watch
├── exercises/                  # 练习数据（打包时嵌入 clings/exercises/）
├── solutions/                  # 参考答案（CI 验证用，不打包进 wheel）
├── tests/                      # Unit 3 公开测试数据（打包时嵌入 clings/tests/）
├── intro/                      # 训练营入门讲义
├── clings.toml                 # Unit 配置（打包时嵌入 clings/clings.toml）
├── pyproject.toml              # 包元数据 + hatchling 构建配置
├── .cnb.yml                    # CNB CI 流水线
├── cliff.toml                  # git-cliff changelog 配置
├── CURRICULUM.md               # 完整课程大纲
├── AGENTS.md                   # 项目 AI 助手指南
└── README.md                   # 本文件
```

### 模块依赖关系

依赖方向严格单向（底层不依赖上层）：

```
cli.py                    ← 入口：argparse + 命令分发
├── config.py             ← 叶节点：路径、发现、异常定义
├── compiler.py           ← 编译测试引擎（依赖 config）
├── state.py              ← 进度持久化（依赖 config）
├── utils.py              ← 共享工具（依赖 config）
├── renderer.py           ← UI 渲染（依赖 state, utils, config）
└── commands/             ← 子命令（依赖上述模块）
    ├── watch.py          ← watch 模式（依赖 compiler, config, state, renderer）
    ├── run.py            ← 单题运行（依赖 compiler, config, state）
    ├── check.py          ← 批量验证（依赖 compiler, config）
    ├── score.py          ← CI 评分（依赖 compiler, config）
    ├── init.py           ← 初始化（依赖 config）
    ├── reset.py          ← 重置（依赖 config, state, utils）
    ├── list.py           ← 列出（依赖 config, state, utils）
    ├── hint.py           ← 提示（依赖 config, state）
    ├── tests.py          ← 测试用例（依赖 config）
    └── doctor.py         ← 环境诊断（依赖 config）
```

### 关键设计决策

**双路径检测**：`config.py` 自动检测运行环境，兼容两种安装模式：

```python
# pip install clings → 包内路径 site-packages/clings/exercises/
# pip install -e .   → 仓库顶层路径 ./exercises/
EXERCISES_DIR = _resolve_pkg_path("exercises")   or _resolve_repo_path("exercises")
PKG_CONFIG    = _resolve_pkg_path("clings.toml") or _resolve_repo_path("clings.toml")
```

**自定义异常**：唯一的 `ClingsError` 异常类，所有用户可见错误统一抛出，由 `cli.py` 的 `main()` 统一捕获并格式化输出。

**Zero-copy 打包**：`exercises/` 和 `clings.toml` 放在仓库顶层便于编辑，通过 hatchling 的 `force-include` 嵌入 wheel 包内路径。

---

## 开发指南

### 环境搭建

```bash
git clone https://cnb.cool/opencamp/learning-nccl/clings.git
cd clings/                       # 默认在 cli 分支
pip install -e .                  # editable 安装（修改代码立即生效）
```

### 运行验证

```bash
# 单元级验证
python -m clings -v               # 版本号
python -m clings doctor           # 环境诊断

# 全量回归测试（使用参考答案）
python -m clings check unit0 --solutions   # Unit 0（9 题）
python -m clings check unit1 --solutions   # Unit 1（40 题）
python -m clings check unit2 --solutions   # Unit 2（24 题）

# CI 评分
python -m clings score unit0 --json
```

> 修改包代码（`clings/` 目录）后，提交前务必运行 Unit 0 + Unit 1 + Unit 2 全量 check。

### CLI 自身测试（pytest）

clings 作为 Python CLI 工具，自身有 pytest 测试套件（位于 `tests_py/`，与 `tests/` 的 C 练习题数据隔离）：

```bash
pip install -e ".[dev]"            # 安装 pytest（dev 依赖，不进发布包）
pytest tests_py/ -v                # 跑全部测试（秒级）
pytest tests_py/ --cov=clings      # 带覆盖率报告
```

测试分三层：单元测试（纯函数）→ 文件系统测试（tmp_path 隔离）→ 集成测试（真实编译 C）。覆盖 `normalize`/`select_exercises`/`WatchState`/`run_cases` 等核心逻辑。详见 [`clings/AGENTS.md`](clings/AGENTS.md) 测试体系章节。

### Commit 规范

格式：`<emoji> <type>(<scope>): <中文主题>`

| 类型 | Emoji | 适用场景 | 示例 |
|------|:-----:|----------|------|
| feat | ✨ | 新功能 | `✨ feat(watch): 支持快捷键跳转` |
| fix | 🐛 | 缺陷修复 | `🐛 fix(compiler): 修复 make 模式路径` |
| refactor | ♻️ | 结构调整 | `♻️ refactor(config): 提取 lesson 解析` |
| docs | 📝 | 文档改动 | `📝 docs(readme): 更新架构图` |
| build | 📦 | 构建/打包 | `📦 build: 升级 hatchling 配置` |
| chore | 🔧 | 工具链/维护 | `🔧 chore: 更新 cliff.toml` |
| ci | 👷 | CI/CD 配置 | `👷 ci: 添加 unit2 流水线` |
| test | ✅ | 测试 | `✅ test: 添加边界用例` |

### 发版流程

版本号由 `git cliff --bumped-version` 根据 commit 类型自动推导：

| Commit 类型 | 版本变化 | 示例 |
|-------------|----------|------|
| `feat` | minor bump | 4.3 → 4.4 |
| `fix` / `refactor` | patch bump | 4.3.0 → 4.3.1 |
| `BREAKING CHANGE` | major bump | 4.x → 5.0 |

触发方式：CNB Web UI「一键发版」按钮，或手动 `git tag vX.Y.Z && git push origin vX.Y.Z`。CI 流水线自动更新 `pyproject.toml`、commit、打 tag、发布到 PyPI。

> ⚠️ **不要手动修改** `pyproject.toml` 中的 `version` 字段。

### 新增 Unit

编辑 `clings.toml`，添加 `[[units]]` 条目即可，CLI 自动适配：

```toml
[[units]]
id = "unit2"
title = "C Essentials"
lessons = "25-48"
```

---

## CI/CD

push 到 `cli` 分支自动触发 CNB 流水线，包含四层并行验证：

```
push → cli 分支
│
├── Unit 0 流水线 (Init → List → Run → Check → Score)
├── Unit 1 流水线 (Init → List → Run → Check → Score)   ← 并行
├── Unit 2 流水线 (Init → List → Run → Check → Score)   ← 并行
├── Test 流水线 (pytest tests_py/ --cov)                ← 并行（CLI 自身测试）
└── Build (python -m build → wheel + sdist)              ← 并行
```

CI 环境：`docker.cnb.cool/opencamp/learning-nccl/opencamp-c-env/ci:latest`（含 GCC 14+ / ncurses / 32 位支持 / Python 3）

发布流程：

```
push (cli)     →  自动化测试  →  python -m build  →  twine upload PyPI
web_trigger    →  一键发版     →  bump version    →  changelog → tag → publish
```

---

## 常见问题

<details>
<summary><b>Q: 需要什么环境？</b></summary>

- Python 3.11 或更高版本
- GCC（或 Clang）编译器
- Linux / macOS / Windows WSL2
- 推荐使用 CNB 云原生开发环境（Fork 仓库后一键启动，免本地配置）

</details>

<details>
<summary><b>Q: Windows 能用吗？</b></summary>

推荐使用 WSL2 + Ubuntu。原生 Windows 下 clings 会尝试检测 MinGW GCC，但体验不如 Linux 环境。训练营推荐使用 CNB 云原生开发环境（浏览器内直接开发）。

</details>

<details>
<summary><b>Q: 和 Rustlings 是什么关系？</b></summary>

Clings 在交互模式、练习组织方式上借鉴了 Rustlings 的设计理念（watch 模式、hint 命令、list 进度），但：
- 面向 C 语言（而非 Rust）
- 评测模式更丰富（return / stdout / compile / make+stdout）
- 与 OpenCamp 训练营课程深度绑定
- 纯 Python 实现，零依赖
- 练习规模更大（约 97 题 vs 94 题）

</details>

<details>
<summary><b>Q: 如何提交作业？</b></summary>

训练营学员通过 CNB 平台提交：
1. Fork 对应 Unit 的练习仓库
2. 使用 clings 在本地（或云环境）完成练习
3. 提交代码到 main 分支并创建 Merge Request
4. CI 自动评分，可多次提交取最高分
5. 查看 OpenCamp 晋级榜单

</details>

<details>
<summary><b>Q: 可以离线使用吗？</b></summary>

可以。Clings 是纯 Python 标准库实现，无网络依赖。安装后即可完全离线使用。`clings init` 从已安装的 wheel 包中释放练习文件，无需联网。

</details>

<details>
<summary><b>Q: 如何更新 clings？</b></summary>

```bash
# uvx 方式（自动使用最新版）
uvx clings@latest

# pipx 方式
pipx upgrade clings

# pip 方式
pip install --upgrade clings
```

</details>

<details>
<summary><b>Q: 如何贡献代码？</b></summary>

1. Clone 仓库，切换到 `cli` 分支
2. `pip install -e .` 安装 editable 模式
3. 修改代码后运行 `python -m clings check unit0 --solutions && python -m clings check unit1 --solutions`
4. 遵循 Commit 规范提交，创建 Merge Request

</details>

<details>
<summary><b>Q: 能用自己的编辑器吗？</b></summary>

当然。Clings 监控文件系统变化，你使用任何编辑器（VS Code / Vim / Emacs / CLion）修改 `.c` 文件，保存后 clings 自动编译验证。也可以使用 CNB 云开发环境内置的 Web IDE。

</details>

---

## 相关资源

| 资源 | 链接 |
|------|------|
| 🏕️ 训练营主页 | [opencamp.cn/C/camp/2026](https://opencamp.cn/C/camp/2026) |
| 📦 PyPI | [pypi.org/project/clings](https://pypi.org/project/clings/) |
| 🔗 源码仓库 | [cnb.cool/opencamp/learning-nccl/clings](https://cnb.cool/opencamp/learning-nccl/clings) |
| 📖 课程大纲 | [CURRICULUM.md](./CURRICULUM.md) |
| 📘 教材推荐 | 《C 程序设计语言》K&R · 《C Primer Plus》 · K.N. King |
| 🛠️ GCC 文档 | [gcc.gnu.org/onlinedocs](https://gcc.gnu.org/onlinedocs/) |
| 🦀 Rustlings | [github.com/rust-lang/rustlings](https://github.com/rust-lang/rustlings) |

---

## Contributors

感谢以下贡献者为 Clings 项目做出的贡献：

| 贡献者 | CNB |
|:------:|-----|
| 椼 | [@miao](https://cnb.cool/u/miao) |
| wwvo | [@illegal_name_cnb.by9cbmyhqda](https://cnb.cool/u/illegal_name_cnb.by9cbmyhqda) |

---

## License

[MIT](LICENSE) © 2026 OpenCamp C Camp Team

---

<!-- badge 引用 -->
[badge-pypi]: https://img.shields.io/pypi/v/clings?style=flat-square
[badge-python]: https://img.shields.io/pypi/pyversions/clings?style=flat-square
[badge-downloads]: https://img.shields.io/pypi/dm/clings?style=flat-square
[badge-license]: https://img.shields.io/pypi/l/clings?style=flat-square

<!-- 链接 -->
[pypi]: https://pypi.org/project/clings/
