## Lesson 44 — qsort 泛型排序：int 与字符串数组

### 课程任务

调用 C 标准库的 `qsort` 完成两种类型的排序，掌握 `void*` 泛型机制与 comparator 函数指针：

1. **int 数组**：使用已提供的 `cmp_int`（一级指针解引用）
2. **字符串数组 `char *names[]`**：你需要实现 `cmp_str` + qsort 调用（**二级指针解引用——本课核心难点**）

```c
int cmp_str(const void *a, const void *b);  /* ← 你来实现 */
```

```
输入 "int 5 3 8 1 4\n"          → ints: 1 3 4 5 8
输入 "str cherry apple banana\n" → strings: apple banana cherry
```

### 前置知识：qsort 签名

```c
void qsort(void *base, size_t n, size_t size,
           int (*cmp)(const void *, const void *));
```

| 参数 | 含义 | int 数组 | 字符串数组 |
|------|------|---------|-----------|
| base | 数组首地址 | `arr` | `strs` |
| n | 元素个数 | 5 | 3 |
| size | 单个元素字节数 | `sizeof(int)`=4 | `sizeof(char*)`=8 |
| cmp | 比较函数指针 | `cmp_int` | `cmp_str` ←你写 |

### cmp 函数万能模板

```
int cmp(const void *a, const void *b) {
    const ELEMENT_TYPE *pa = a;  // 强转
    const ELEMENT_TYPE *pb = b;
    return COMPARE(*pa, *pb);     // 解引用+比较
}
```

**核心规律**：qsort 总是传"元素的地址"。元素是什么类型，就强转为什么指针，然后解引用一次。

### 一级指针：int 数组（简单）

```
int arr[] = {5, 3, 8, 1, 4};
qsort(arr, 5, sizeof(int), cmp_int);

qsort 工作时，传给 cmp 的是:
  &arr[0], &arr[1], ... → 类型是 int*

所以 cmp_int:
  const int *pa = (const int *)a;   // void* → int*
  return *pa - *pb;                  // 解引用一次 → 得到 int
```

```
内存视角:
  arr[0]=5    arr[1]=3    arr[2]=8
  ┌──────┐    ┌──────┐    ┌──────┐
  │  5   │    │  3   │    │  8   │    ← sizeof(int)=4
  └──────┘    └──────┘    └──────┘
  ↑0x100       ↑0x104       ↑0x108        ← 元素的地址
  &arr[0]=int*  &arr[1]=int*  &arr[2]=int*

qsort 传给 cmp 的是 0x100, 0x104, ... (int*)
cmp 强转 void* → int*, 解引用 → 拿到 int 值 ✓
```

### 二级指针：字符串数组（重点！难点！）

```
char *strs[] = {"cherry", "apple", "banana"};

strs 的元素是什么？是 char*（指向字符串的指针）!
qsort 传给 cmp 的是元素的地址 → &strs[i] → 类型是 char**！
```

**完整内存模型**：

```
栈上的 strs 数组:                  堆上的字符串:
┌──────────┐                      ┌───┬───┬───┬───┬───┬───┬───┐
│ 0x200 ──┼────────────────────→ │ c │ h │ e │ r │ r │ y │ \0│
└──────────┘                      └───┴───┴───┴───┴───┴───┴───┘
  strs[0]   (地址 0xA00)            ↑ 0x200 (字符串地址)

┌──────────┐                      ┌───┬───┬───┬───┬───┬───┐
│ 0x300 ──┼────────────────────→ │ a │ p │ p │ l │ e │ \0│
└──────────┘                      └───┴───┴───┴───┴───┴───┘
  strs[1]   (地址 0xA08)            ↑ 0x300

┌──────────┐                      ┌───┬───┬───┬───┬───┬───┬───┐
│ 0x400 ──┼────────────────────→ │ b │ a │ n │ a │ n │ a │ \0│
└──────────┘                      └───┴───┴───┴───┴───┴───┴───┘
  strs[2]   (地址 0xA10)            ↑ 0x400

qsort 传给 cmp 的是:
  &strs[0] = 0xA00  → 类型是 char**
  &strs[1] = 0xA08  → 类型是 char**
  &strs[2] = 0xA10  → 类型是 char**

所以 cmp_str 必须:
  const char **pa = (const char **)a;   // void* → char**
  return strcmp(*pa, *pb);              // *pa = strs[i] = char*
```

### 为什么 `(const char *)a` 是死亡陷阱？

```c
/* ❌ 错误写法 —— 考场上最常见的 bug */
int cmp_str(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

/* 发生了什么？
   qsort 传 a = &strs[0] = 0xA00
   (const char *)a = (const char *)0xA00 = 0xA00
   
   但 0xA00 处存储的是一个指针值 0x200（strs[0] 的内容），
   不是字符串 "cherry"！
   
   strcmp 把 0xA00 处的 8 字节二进制数据当作字符串去读 →
   读到的是指针值的字节表示（如 0x00 0x02 0x00...），
   很可能立即遇到 \0 → 视为空串 → 排序完全乱掉 → 段错误！
*/

/* ✅ 正确写法 */
int cmp_str(const void *a, const void *b) {
    const char *s1 = *(const char **)a;  // 先解引用: 拿到 char*
    const char *s2 = *(const char **)b;
    return strcmp(s1, s2);
}
```

### 泛型解引用对照表

| 数组类型 | 元素类型 | sizeof(元素) | qsort 传的地址类型 | cmp 强转为 | 解引用得到 |
|---------|---------|-------------|-------------------|-----------|-----------|
| `int arr[]` | int | 4 | `int*` | `const int*` | int |
| `double arr[]` | double | 8 | `double*` | `const double*` | double |
| `char arr[]` | char | 1 | `char*` | `const char*` | char |
| `char *arr[]` | char* | 8 | **`char**`** | **`const char**`** | char* |
| `struct node *arr[]` | struct node* | 8 | `struct node**` | `const struct node**` | struct node* |

**记忆口诀**：qsort 传"元素地址"——元素什么类型，指针就几级。
- int 数组 → 元素=int → 元素地址=int* → 一级指针
- char* 数组 → 元素=char* → 元素地址=char** → 二级指针

### qsort 内部如何工作？

```
qsort 不关心元素类型。它做三件事:
1. Compare: 调用 cmp(&arr[i], &arr[j]) → 你写的 cmp 决定谁大谁小
2. Swap: 用 memcpy 交换 size 字节 → 类型透明
3. Recursion/Iteration: 对左右子数组重复

void* + size 参数 + cmp 函数指针 = C 语言的泛型编程模式
这就是没有模板的年代里 C 实现"一套代码，多种类型"的方式。
```

### 常见错误

| 错误 | 后果 | 正确 |
|------|------|---------|
| cmp_str 用 `(const char *)a` | 把指针值当字符串读 | `*(const char **)a` |
| qsort 的 size 写成 strlen | 交换时破坏内存 | `sizeof(char *)` |
| cmp_int 的 `*a - *b` 溢出 | INT_MIN-1 溢出 | 用 if-else 比较 |
| 忘 `#include <stdlib.h>` | qsort 未声明 | 必须 include |
| cmp 参数忘 `const` | 编译器警告 | `const void*` 接收 |

### 重要知识点

- qsort 通过 `void*` + `sizeof(element)` + `cmp` 实现无类型的泛型排序
- cmp 核心原则：qsort 传"元素地址"，元素类型决定了需要几级指针
- 字符串数组的元素是 `char*` → 元素地址是 `char**` → 需要二级解引用
- qsort 不保证稳定性——C 标准未规定

### 课堂讨论

1. **为什么 int 数组一级、字符串数组二级？** — int 数组元素就是 int → 元素地址 int* → 一级。字符串数组元素是 char* → 元素地址 char** → 二级。qsort 总是在"元素地址"层面操作。

2. **qsort 稳定吗？** — C 标准未规定。glibc 对小数组用归并排序（稳定），大数组用快排（不稳定）。不要依赖这个行为。

3. **`(const char *)a` 为何段错误？** — qsort 传的是 `&strs[i]`（元素的地址），不是 `strs[i]`（元素的值）。不先解引用就无法拿到真正的字符串指针。

4. **三级指针什么时候出现？** — `char **argv[]`（字符串数组的数组）→ 元素=char** → 地址=char*** → 三级指针。不过 qsort 很少用到三级。

5. **手写快排和 qsort 哪个快？** — 手写快排可内联比较器，无函数指针开销。但 qsort 的实现经过高度优化（introsort、小数组插排），差距通常可忽略。

### 后续衔接

- Lesson 41: 手写快排——理解 qsort 内部的分区+递归原理
- Lesson 45: realloc——另一个需要理解内存模型的 API
- C++ 对比: `std::sort` 用模板消除 void* 和函数指针开销

### 完整示例输出追踪

```
输入: "str cherry apple banana\n"

strtok 解析: mode="str", tokens = ["cherry", "apple", "banana"]
strs = ["cherry", "apple", "banana"]  (n=3)

qsort 前: {"cherry", "apple", "banana"}

qsort 内部调用 cmp_str 的过程:
  cmp_str(&strs[0], &strs[1]):
    → strcmp("cherry", "apple") → 'c' > 'a' → 正数 → 交换!

  cmp_str(&strs[0], &strs[2]):
    → strcmp("banana", "apple") → 'b' > 'a' → 正数 → 交换!

  cmp_str(&strs[1], &strs[2]):
    → strcmp("cherry", "banana") → 'c' > 'b' → 正数 → 交换!

qsort 后: {"apple", "banana", "cherry"}

输出: strings: apple banana cherry ✓
```

### qsort 内部实现（了解即可）

glibc 的 qsort 通常实现为 introsort（内省排序）：
- 快速排序为主（选三数取中 pivot）
- 递归深度超过 2×log₂n 时切换到堆排序（防 O(n²)）
- 子数组小于一定阈值（通常 4-16）时切换到插入排序

这就是为什么 qsort 在实际使用中几乎不会出现 O(n²) 退化。

### C 泛型 vs C++ 模板

| | C qsort | C++ std::sort |
|---|--------|--------------|
| 泛型方式 | void* + 函数指针 | 模板特化 |
| 类型安全 | 编译期不检查 | 编译期完全检查 |
| 比较开销 | 函数指针间接调用 | 内联展开（零开销） |
| 代码膨胀 | 只有一份实现 | 每种类型生成一份 |
| 速度 | 较慢（函数调用开销） | 更快（内联优化） |

### 参考资料

- `man qsort` — 标准库文档
- C 标准 §7.20.5 — qsort 规范
- Bentley & McIlroy (1993) "Engineering a Sort Function"
- Musser (1997) "Introspective Sorting and Selection Algorithms"
- Sedgewick "Algorithms" §2.3 — Quicksort optimizations
- glibc `qsort.c` 源码 — 工业级 introsort 实现
