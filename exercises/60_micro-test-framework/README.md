# Lesson 60 — 微型 C 单元测试框架：用宏构建测试系统（含 NULL 处理）

### 课程任务

用纯 C 预处理器宏实现一个微型单元测试框架，包含四个核心宏和五个自测用例：

- **ASSERT_EQ(a, b)**: 整数相等断言，失败时打印文件、行号、期望值和实际值
- **ASSERT_STREQ(a, b)**: 字符串相等断言，正确处理 NULL 指针（双 NULL 视为相等），失败时打印两个字符串
- **TEST(name) { ... }**: 定义测试用例，用 `##` 记号粘贴生成 `test_##name` 函数
- **RUN_TESTS()**: 运行所有注册的测试，打印每个测试的 PASS/FAIL 状态和最终汇总

写五个测试用例验证框架正确性：

```
./test_framework
Running 5 test(s)...

[test_add] PASS
[test_str] PASS
[test_fail]   FAIL test_framework.c:88: ASSERT_EQ(2 + 2, 5) — expected 5, got 4
[test_null]   FAIL test_framework.c:96: ASSERT_STREQ(NULL, "hello") — expected "hello", got "(null)"
[test_float]   FAIL test_framework.c:107: ASSERT_EQ((int)2.718, 3) — expected 3, got 2

─── Summary ───
5 tests, 2 passed, 3 failed
```

其中 `test_fail` 和 `test_float` 是故意写错的断言，`test_null` 同时包含通过和失败的 NULL 断言，用于全面验证框架的失败报告功能。

> **关于行号**：上面示例里的 `88 / 96 / 107` 是参考实现中这三条失败断言所在的源码行号。**你的实现行号很可能不同，判分不校验具体行号**（只校验 `test_framework.c:<行号>:` 这个格式以及断言表达式、期望/实际值、汇总行）。另外 `#` 字符串化会原样保留源码空格，失败断言请照判分规格写成 `ASSERT_EQ(2 + 2, 5)`（加号两侧留空格）。用 `clings tests 60` 查看完整判分规格。

### 前置知识

#### 单元测试概念

单元测试（Unit Testing）是软件工程中验证最小代码单元（通常是单个函数）正确性的实践。与集成测试和端到端测试不同，单元测试运行速度快、隔离性好、定位问题精确。

```
测试金字塔 (Testing Pyramid):
            ┌─────────┐
            │   E2E   │  ← 少量端到端测试（模拟用户操作）
           ┌┴─────────┴┐
           │ Integration│ ← 中量集成测试（模块间交互）
          ┌┴───────────┴┐
          │    Unit      │ ← 大量单元测试（单个函数）★ 本题实现
          └──────────────┘
```

TDD（Test-Driven Development）红 - 绿 - 重构循环：

```
  ┌──────────────────────────────────┐
  │                                  │
  ▼                                  │
┌──────┐    ┌──────────┐    ┌──────────────┐
│ RED  │───▶│  GREEN   │───▶│   REFACTOR   │
│ 写失败 │    │ 最少代码  │    │ 优化结构     │
│ 的测试 │    │ 让测试通过│    │ 测试保持绿色  │
└──────┘    └──────────┘    └──────────────┘
```

#### C 预处理器三大核心操作符

**1. `#` — 字符串化操作符 (Stringification)**

将宏参数转换为 C 字符串字面量（加双引号）。这是断言宏能打印表达式文本的关键。

```c
#define STR(x) #x

STR(hello)       /* 展开为 "hello"        */
STR(1 + 2)       /* 展开为 "1 + 2"        */
STR(assert_eq)   /* 展开为 "assert_eq"    */
```

在 ASSERT_EQ 中的应用：

```c
#define ASSERT_EQ(a, b) do { \
    if ((a) != (b))          \
        printf("FAIL: %s != %s\n", #a, #b); \
} while(0)

ASSERT_EQ(x + 1, 5);
/* 输出：FAIL: x + 1 != 5   ← 打印的是表达式文本，不是值！*/
```

**2. `##` — 记号粘贴操作符 (Token Pasting)**

将两个预处理记号合并为一个新记号。这是 TEST 宏能动态生成函数名的关键。

```c
#define CONCAT(a, b) a ## b
#define MAKE_FUNC(name) void func_##name(void)

CONCAT(my_, var)     /* 展开为 my_var       */
MAKE_FUNC(add)       /* 展开为 void func_add(void) */

/* TEST 宏的核心原理 */
#define TEST(name) static void test_##name(void)

TEST(add) { }
/* 展开为：static void test_add(void) { } */
```

**3. `do { ... } while(0)` — 多语句宏惯用法**

这是 C 预处理器编程中最重要的惯用法。当一个宏需要包含多条语句时，必须用 `do { ... } while(0)` 包裹，否则在 if/else 等控制流中会出错。

```
错误示范 — 不用 do-while(0) 的灾难:
┌─────────────────────────────────────────────────────────┐
│ #define FOO() stmt1(); stmt2();                         │
│                                                         │
│ if (cond)                                               │
│     FOO();    ← 展开为 if (cond) stmt1(); stmt2();      │
│ else                                                    │
│     bar();    ← 语法错误！else 找不到匹配的 if            │
│                                                         │
│ 正确做法:                                                │
│ #define FOO() do { stmt1(); stmt2(); } while(0)         │
│                                                         │
│ if (cond)                                               │
│     do { stmt1(); stmt2(); } while(0);  ← 一条完整语句    │
│ else                                                    │
│     bar();    ← 正确！                                   │
└─────────────────────────────────────────────────────────┘
```

#### 函数指针与回调机制

本框架的测试注册表使用函数指针数组，这是 C 语言实现多态和回调的基础：

```c
typedef struct {
    void (*func)(void);   /* 函数指针：指向 void → void 的函数 */
    const char *name;     /* 测试名称 */
} TestEntry;

TestEntry tests[64];      /* 静态数组存储所有测试 */
```

函数指针语法说明：

| 声明               | 含义                                   |
| ------------------ | -------------------------------------- |
| `void (*fp)(void)` | fp 是指向 `void f(void)` 的指针        |
| `void *fp(void)`   | fp 是返回 `void*` 的函数（不是指针！） |
| `fp = &my_func`    | 取函数地址                             |
| `fp = my_func`     | 函数名自动退化为指针（等价）           |
| `fp()`             | 通过指针调用函数                       |

#### 静态全局变量与内部链接

本框架用 `static` 声明全局变量和函数，确保它们只在当前编译单元可见：

```c
static int test_count = 0;   /* 仅本文件可见 */
static int fail_count = 0;   /* 仅本文件可见 */
```

这样即使多个 `.c` 文件各自包含测试框架，也不会产生符号冲突。

### 算法详解：框架运行流程

#### 整体架构图

```
┌──────────────────────────────────────────────────────┐
│                    Test Framework                     │
│                                                      │
│  ┌──────────┐   ┌──────────┐   ┌──────────┐         │
│  │ TEST(add)│   │ TEST(str)│   │TEST(fail)│         │
│  │ {        │   │ {        │   │ {        │         │
│  │ ASSERT_EQ│   │ ASSERT_  │   │ ASSERT_EQ│         │
│  │ (1+1,2); │   │ STREQ(   │   │ (2+2,5); │         │
│  │ ...      │   │ "h","h");│   │ }        │         │
│  │ }        │   │ ...      │   │          │         │
│  └────┬─────┘   │ }        │   └────┬─────┘         │
│       │         └────┬─────┘        │               │
│  ┌────┴─────┐   ┌────┴─────┐   ┌────┴─────┐         │
│  │TEST(null)│   │TEST(float)│   │          │         │
│  │ {        │   │ {        │   │          │         │
│  │ ASSERT_  │   │ ASSERT_EQ│   │          │         │
│  │ STREQ(   │   │ ((int)   │   │          │         │
│  │ NULL,    │   │  3.14,3);│   │          │         │
│  │ NULL);   │   │ ASSERT_EQ│   │          │         │
│  │ ASSERT_  │   │ ((int)   │   │          │         │
│  │ STREQ(   │   │ 2.718,3);│   │          │         │
│  │ NULL,    │   │ }        │   │          │         │
│  │ "hello");│   └────┬─────┘   │          │         │
│  │ }        │        │         │          │         │
│  └────┬─────┘        │         │          │         │
│       ▼              ▼         ▼          ▼         │
│  ┌────────────────────────────────────┐             │
│  │        TestEntry tests[64]         │             │
│  │  [0] func→test_add, name="test_add"│             │
│  │  [1] func→test_str, name="test_str"│             │
│  │  [2] func→test_fail,name="test_fail"│            │
│  │  [3] func→test_null,name="test_null"│            │
│  │  [4] func→test_float,name="test_float"│          │
│  └──────────────┬─────────────────────┘             │
│                 │                                    │
│                 ▼                                    │
│  ┌─────────────────────────────────────┐            │
│  │          RUN_TESTS()                │            │
│  │  for i = 0..test_index-1:           │            │
│  │    before = fail_count              │            │
│  │    tests[i].func()     ← 调用测试    │            │
│  │    if fail_count == before → PASS   │            │
│  │    else                     → (已打印FAIL)       │
│  │  print summary                      │            │
│  └─────────────────────────────────────┘            │
└──────────────────────────────────────────────────────┘
```

#### 断言失败时的控制流

当 ASSERT_EQ 检测到不匹配时，关键行为是 `return`——它从当前 TEST 函数中返回：

```
TEST(something) {
    ASSERT_EQ(a, b);  ← 如果失败: 打印 FAIL → fail_count++ → return
    ASSERT_EQ(c, d);  ← 此行不会执行！（上一个断言已 return）
}
```

这保证了每个测试函数的"快速失败"（fail-fast）语义：第一个失败的断言终止该测试，后续断言不再执行。这避免了级联错误——后面的断言失败可能只是前面失败的结果。

#### ASSERT_STREQ 的 NULL 处理逻辑

这是本框架最微妙的逻辑之一。正确处理 NULL 指针的语义：

```
NULL 比较真值表:
┌──────────┬──────────┬──────────┐
│    a     │    b     │  结果    │
├──────────┼──────────┼──────────┤
│  NULL    │  NULL    │  PASS    │  ← 双 NULL 视为相等
│  NULL    │ "hello"  │  FAIL    │  ← 仅一个为 NULL
│ "hello"  │  NULL    │  FAIL    │  ← 仅一个为 NULL
│ "hello"  │ "hello"  │  PASS    │  ← strcmp == 0
│ "hello"  │ "world"  │  FAIL    │  ← strcmp != 0
└──────────┴──────────┴──────────┘
```

实现要点：

```c
/* 错误写法 — 双 NULL 也失败！ */
if (_aa == NULL || _bb == NULL || strcmp(_aa, _bb) != 0)

/* 正确写法 — 先判断 NULL 状态是否一致 */
if ((_aa == NULL) != (_bb == NULL) ||
    (_aa != NULL && _bb != NULL && strcmp(_aa, _bb) != 0))
```

第一种写法中 `_aa == NULL` 在两者都是 NULL 时也为真，导致错误地报告失败。第二种写法用 XOR 逻辑判断"是否只有一个为 NULL"，再在两者都非 NULL 时用 strcmp 比较。

#### ASSERT_EQ 与浮点数的关系

ASSERT_EQ 直接比较整数值，对 `double`/`float` 类型会被隐式转换为 `int`（截断小数部分）：

```
ASSERT_EQ((int)3.14, 3)    → (int)3 == 3  → PASS
ASSERT_EQ((int)2.718, 3)   → (int)2 == 3  → FAIL
```

这展示了整数断言在浮点场景下的局限性。成熟的测试框架提供专门的 `ASSERT_DOUBLE_EQ(a, b, epsilon)` 宏，使用容差比较：

```c
#define ASSERT_DOUBLE_EQ(a, b, eps) do { \
    double _da = (a), _db = (b);          \
    if (fabs(_da - _db) > (eps)) { ... } \
} while(0)
```

#### 宏展开示例：完整追踪

以 `ASSERT_EQ(2 + 2, 5)` 为例，展示从源代码到最终输出的全过程：

```
Step 1 — 源代码:
  ASSERT_EQ(2 + 2, 5);

Step 2 — 宏展开（a=2+2, b=5）:
  do {
      if ((2 + 2) != (5)) {
          printf("  FAIL %s:%d: ASSERT_EQ(%s, %s) "
                 "— expected %d, got %d\n",
                 __FILE__, __LINE__,
                 "2 + 2", "5", (int)(5), (int)(2 + 2));
          fail_count++;
          return;
      }
  } while(0);

Step 3 — 编译后执行:
  (2+2)=4, (5)=5, 4!=5 → 进入 if 分支
  __FILE__ = "test_framework.c"
  __LINE__ = 87
  #a = "2 + 2", #b = "5"
  (int)(b) = 5, (int)(a) = 4

Step 4 — 输出:
    FAIL test_framework.c:87: ASSERT_EQ(2 + 2, 5) — expected 5, got 4
```

同样追踪 `ASSERT_STREQ(NULL, "hello")` 的展开：

```
Step 1 — 源代码:
  ASSERT_STREQ(NULL, "hello");

Step 2 — 宏展开:
  do {
      const char *_aa = (NULL);
      const char *_bb = ("hello");
      if ((_aa == NULL) != (_bb == NULL) ||
          (_aa != NULL && _bb != NULL && strcmp(_aa, _bb) != 0)) {
          printf("  FAIL %s:%d: ASSERT_STREQ(%s, %s) "
                 "— expected \"%s\", got \"%s\"\n",
                 __FILE__, __LINE__,
                 "NULL", "\"hello\"",
                 _bb ? _bb : "(null)",
                 _aa ? _aa : "(null)");
          fail_count++;
          return;
      }
  } while(0);

Step 3 — 关键细节:
  (_aa == NULL) = 1 (true),  (_bb == NULL) = 0 (false)
  (1 != 0) = 1 → 进入失败分支
  三元运算符 _bb ? _bb : "(null)" 安全处理 NULL 字符串。

Step 4 — 输出:
    FAIL test_framework.c:96: ASSERT_STREQ(NULL, "hello") — expected "hello", got "(null)"
```

### 完整逐步跟踪

以 5 个测试用例的运行全过程为例：

| Step | 操作                            | test_count | fail_count | test_index | 输出                          |
| ---- | ------------------------------- | ---------- | ---------- | ---------- | ----------------------------- |
| 0    | 初始状态                        | 0          | 0          | 0          | —                             |
| 1    | register_test(test_add)         | 0          | 0          | 1          | —                             |
| 2    | register_test(test_str)         | 0          | 0          | 2          | —                             |
| 3    | register_test(test_fail)        | 0          | 0          | 3          | —                             |
| 4    | register_test(test_null)        | 0          | 0          | 4          | —                             |
| 5    | register_test(test_float)       | 0          | 0          | 5          | —                             |
| 6    | RUN_TESTS 开始                  | 5          | 0          | 5          | "Running 5 test(s)..."        |
| 7    | 运行 tests[0]="test_add"        | 5          | 0          | 5          | "[test_add] "                 |
| 7a   | 3 个 ASSERT_EQ 全部通过         | 5          | 0          | 5          | —                             |
| 7b   | fail_count 未变 →               | 5          | 0          | 5          | "PASS"                        |
| 8    | 运行 tests[1]="test_str"        | 5          | 0          | 5          | "[test_str] "                 |
| 8a   | 3 个 ASSERT_STREQ 全部通过      | 5          | 0          | 5          | —                             |
| 8b   | fail_count 未变 →               | 5          | 0          | 5          | "PASS"                        |
| 9    | 运行 tests[2]="test_fail"       | 5          | 0          | 5          | "[test_fail] "                |
| 9a   | ASSERT_EQ(2+2,5) 失败           | 5          | 1          | 5          | " FAIL ..."                   |
| 9b   | return（不打印 PASS）           | 5          | 1          | 5          | —                             |
| 10   | 运行 tests[3]="test_null"       | 5          | 1          | 5          | "[test_null] "                |
| 10a  | ASSERT_STREQ(NULL,NULL) 通过    | 5          | 1          | 5          | —                             |
| 10b  | ASSERT_STREQ(NULL,"hello") 失败 | 5          | 2          | 5          | " FAIL ..."                   |
| 10c  | return（不打印 PASS）           | 5          | 2          | 5          | —                             |
| 11   | 运行 tests[4]="test_float"      | 5          | 2          | 5          | "[test_float] "               |
| 11a  | ASSERT_EQ((int)3.14,3) 通过     | 5          | 2          | 5          | —                             |
| 11b  | ASSERT_EQ((int)2.718,3) 失败    | 5          | 3          | 5          | " FAIL ..."                   |
| 11c  | return（不打印 PASS）           | 5          | 3          | 5          | —                             |
| 12   | 打印汇总                        | 5          | 3          | 5          | "5 tests, 2 passed, 3 failed" |

### 断言对比表

| 特性       | ASSERT_EQ               | ASSERT_STREQ                    | TEST                     | RUN_TESTS            |
| ---------- | ----------------------- | ------------------------------- | ------------------------ | -------------------- |
| 用途       | 整数相等断言            | 字符串相等断言                  | 定义测试用例             | 执行所有测试         |
| 参数       | (a, b) 两个整数表达式   | (a, b) 两个字符串               | (name) 测试名称          | 无参数               |
| 失败行为   | 打印期望/实际值，return | 打印期望/实际字符串，return     | N/A（定义用）            | N/A（执行用）        |
| 关键宏技巧 | # 字符串化              | # 字符串化 + 局部变量防重复求值 | ## 记号粘贴              | do-while(0) 包装循环 |
| 空指针安全 | 不适用（整数）          | 双 NULL 通过，单 NULL 失败      | N/A                      | N/A                  |
| 典型错误   | 忘加括号导致优先级错误  | NULL 逻辑错误导致双 NULL 也失败 | 忘写 static 导致链接冲突 | 忘写 do-while(0)     |

### 常见错误

| 错误                                                                 | 后果                                           | 正确做法                                              |
| -------------------------------------------------------------------- | ---------------------------------------------- | ----------------------------------------------------- |
| `#define ASSERT_EQ(a,b) if((a)!=(b)) printf(...)` 忘写 do-while(0)   | 在 if/else 后展开导致语法错误                  | 用 `do { ... } while(0)` 包裹整个宏体                 |
| `ASSERT_EQ(x++, 5)` — 宏参数有副作用                                 | 参数被求值多次导致 x 意外递增                  | 断言中避免使用带副作用的表达式                        |
| `ASSERT_STREQ(strcmp(a,b), 0)` 用 ASSERT_EQ 比较字符串               | 编译通过但比较的是 strcmp 的返回值（可能误用） | 使用专门的 ASSERT_STREQ 宏                            |
| `TEST(name) void test_##name(void)` 忘写 static                      | 多个 .c 文件中同名测试函数链接冲突             | 始终用 `static void test_##name(void)`                |
| `ASSERT_STREQ(a, b)` 中直接写 `strcmp(a, b)` 而不存局部变量          | a 或 b 如果是函数调用或复杂表达式会被求值多次  | 用 `const char *_aa = (a)` 先存储                     |
| `#define ASSERT_EQ(a,b) if((a)!=(b)) return;` 忘记递增 fail_count    | 测试失败但不计入统计，汇总显示全部通过         | 失败时先 `fail_count++` 再 `return`                   |
| ASSERT_STREQ 中 `_aa == NULL \|\| _bb == NULL \|\| strcmp(...) != 0` | 双 NULL 也被判定为失败                         | 用 XOR 逻辑：`(_aa==NULL) != (_bb==NULL)`             |
| 在 TEST 函数中直接调用 `return`（非断言触发）                        | 跳过后续断言但不打印失败，汇总结果错误         | 不要在 TEST 中手动 return，让断言控制流程             |
| 断言中用 `#a` 但参数本身含逗号                                       | 预处理器将逗号视为参数分隔符                   | 用括号包裹含逗号的表达式，或避免复杂表达式            |
| 忘记 `#include <math.h>`                                             | test_float 使用浮点常量时可能产生警告          | 包含 math.h（虽然本题不直接调用数学函数，但良好实践） |

### 重要知识点

1. **`#` 操作符在宏展开阶段工作**：它发生在编译之前，将源代码文本转为字符串。因此 `#a` 得到的是参数的**文本形式**而非运行时值。这意味着你无法用 `#` 打印变量的当前值——它永远打印源代码中写的那个标识符。

2. **`do { ... } while(0)` 是零成本抽象**：编译器能识别这个惯用法并完全消除 `do-while` 循环，生成的机器码与直接写语句完全相同。它仅影响语法分析阶段。

3. **`__FILE__` 和 `__LINE__` 是编译器预定义宏**：它们在编译时展开为当前源文件名（字符串）和行号（整数）。断言宏利用它们自动定位失败位置。

4. **函数指针的调用开销极小**：通过 `tests[i].func()` 调用测试函数的开销仅是一次间接跳转（1-2 个 CPU 周期），完全可以忽略。这正是 C 语言"零开销抽象"哲学的体现。

5. **静态数组的测试注册表是最简设计**：本框架用固定大小的 `TestEntry tests[64]` 数组存储测试，无需动态内存分配。成熟框架（如 Google Test）使用自动注册机制，依赖全局构造函数，但 C 语言不支持，因此手动 `register_test()` 是最清晰的方案。

6. **`return` 而非 `exit` 的语义选择**：断言失败时用 `return` 从 TEST 函数返回（而非 `exit()` 终止整个程序），确保 RUN_TESTS 能继续执行剩余测试。每个测试是独立的——一个失败不应阻止其他测试运行。

7. **字符串断言中的 NULL 检查至关重要**：`strcmp(NULL, ...)` 会导致段错误。ASSERT_STREQ 必须在调用 `strcmp` 之前检查 `_aa == NULL || _bb == NULL`，而且要用 XOR 逻辑确保双 NULL 视为相等。这是健壮测试框架的必备特性。

8. **宏参数的括号保护是防御性编程**：写 `#define ASSERT_EQ(a, b)` 时，宏体内的每次参数使用都必须写成 `(a)` 和 `(b)`。即使当前用例没问题，调用者可能传入 `x & 1` 这样的表达式——不加括号会导致运算符优先级错误。

9. **浮点比较的特殊性**：ASSERT_EQ 是整数比较宏，不能直接用于浮点数的精确比较。`0.1 + 0.2 == 0.3` 在浮点运算中通常为假（由于 IEEE 754 精度限制）。成熟的测试框架提供带容差（epsilon）的浮点断言：`ASSERT_DOUBLE_EQ(a, b, 1e-9)`。

### 课堂讨论

1. **为什么不用 `exit(1)` 而是 `return`？**
   `exit(1)` 会立即终止整个进程，RUN_TESTS 无法继续执行后续测试。用 `return` 只退出当前 TEST 函数，控制权回到 RUN_TESTS 的循环中，确保所有测试都有机会运行。这在调试时特别重要——你希望看到所有失败，而不是只看到第一个。

2. **如果 ASSERT_EQ 的参数本身含有逗号怎么办？**
   例如 `ASSERT_EQ(max(1,2), 3)`——预处理器会将 `max(1` 视为第一个参数，`2)` 视为第二个。解决方案：C 预处理器对括号内的逗号"免疫"，所以 `ASSERT_EQ((max(1,2)), 3)` 是安全的。实际使用中，建议避免在断言中直接调用函数，而是先存到变量中。

3. **为什么 test_count 在 RUN_TESTS 开始时赋值而不是在 register_test 中递增？**
   test_count 表示"已运行的测试函数总数"而非"断言总数"。如果每个 ASSERT 都递增 test_count，汇总会变成"12 tests, 9 passed, 3 failed"（3+3+1+2+2 = 11 个断言），语义上不直观。当前设计让 test_count = 注册的 TEST 函数数量（5 个）。

4. **这个框架和 Google Test / Unity / Check 等成熟框架的差距在哪？**
   主要差距：(1) 无自动测试发现——需要手动 register_test；(2) 无测试夹具（fixture）——不能在测试间共享 setup/teardown；(3) 无参数化测试；(4) 断言类型有限（只有整数和字符串）；(5) 无浮点容差断言；(6) 无 XML/JSON 报告输出。但这些简化正是教学价值所在——你完全理解每个机制的工作原理。

5. **为什么 ASSERT_STREQ 要用局部变量存储参数？**
   防止重复求值（double evaluation）。如果直接写 `strcmp((a), (b))`，a 或 b 如果是函数调用（如 `get_string()`）会被调用两次。更危险的是，如果 a 是 `buf++`，第二次求值会得到不同的值。用 `const char *_aa = (a)` 确保参数只求值一次。

6. **为什么 ASSERT_STREQ(NULL, NULL) 应该通过？**
   从语义上讲，两个空指针表示"两个都不存在的字符串"，它们的状态是相同的。这符合"相等"的直觉。如果双 NULL 被判定为不相等，测试代码中就必须写额外的 NULL 检查才能安全使用 ASSERT_STREQ，降低了宏的可用性。

7. **ASSERT_EQ 能否用于浮点数比较？如果不能，应该如何扩展？**
   直接比较浮点数 `ASSERT_EQ(0.1+0.2, 0.3)` 几乎肯定失败（IEEE 754 精度问题）。本题的 test_float 展示了通过 `(int)` 强制转换来比较整数部分。真正的解决方案是添加 `ASSERT_DOUBLE_EQ(a, b, epsilon)` 宏，使用 `fabs((a)-(b)) < epsilon` 进行容差比较。

8. **能否让 TEST 宏自动注册，避免手动 register_test？**
   在标准 C 中很难做到。C++/Google Test 利用全局对象构造函数在 `main()` 之前自动注册，但 C 没有构造函数机制。一种变通方案是用 `__attribute__((constructor))`（GCC 扩展），但不可移植。手动注册虽然略显繁琐，但清晰透明——你明确知道测试的注册顺序和时机。

### 后续衔接

**前置课程**：

- Lesson 20（预处理器）：`#define`、`#include`、条件编译等宏基础
- 函数指针（如 Lesson 44 qsort 中的比较函数）：`void (*fp)(void)` 是理解测试注册表的前提
- `static` 全局变量的内部链接属性
- 结构体与数组（如 Lesson 30-38 的链表/树节点）：`TestEntry` 结构体定义和数组

**后续应用**：

- Lesson 61+（C 全景项目）：在大型 C 项目中使用测试框架验证代码
- 实际工程：本框架可直接扩展为嵌入式 C 项目的轻量测试方案（Unity 框架的核心思想一致）
- 面试：宏编程和函数指针是 C 语言面试的高频考点

### 参考资料

1. **GCC Preprocessor Manual** — `#` (Stringification) 和 `##` (Concatenation) 的官方文档
   https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html

2. **The C Programming Language (K&R)** — 第 4.11.2 节 "Macro Substitution"，第 5.11 节 "Pointers to Functions"

3. **Unity Test Framework** — 轻量级 C 单元测试框架，与本框架设计理念一脉相承
   http://www.throwtheswitch.org/unity

4. **C Preprocessor Trick: do { ... } while(0)** — 多语句宏惯用法的详细解释
   https://stackoverflow.com/questions/154136/do-while-and-if-else-statements-in-c-c-macros

5. **Test-Driven Development: By Example (Kent Beck)** — TDD 红 - 绿 - 重构循环的经典著作

6. **IEEE 754 Floating-Point Arithmetic** — 理解浮点比较为何需要容差（epsilon）
   https://docs.oracle.com/cd/E19957-01/806-3568/ncg_goldberg.html
