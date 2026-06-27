## Lesson 45 — realloc 动态扩容实验

### 课程任务

实现安全的内存动态扩容循环：

1. `malloc` 分配初始容量为 2 的数组
2. 循环读入数字，容量不够时用 `realloc` 扩容（加倍策略）
3. 每次扩容前后用 `uintptr_t` 保存地址并打印 `%p`，判断原地/异地
4. **掌握安全用法**：为什么 `arr = realloc(arr, size)` 是内存泄漏陷阱

```
验证用例:
  输入 "1 2 3 4 5\n" → 展示扩容过程 + in-place/moved + values: 1 2 3 4 5
  输入 "10 20\n"     → 无扩容, values: 10 20
```

### 前置知识：malloc 和 realloc 的行为

```
malloc(n):
  ┌───────────┐
  │ n 字节     │  返回首地址。内容未初始化。
  └───────────┘

realloc(ptr, new_size):
  ┌─── 原数据 ────┬── 新增空间 ────┐
  │ 保留原有内容    │ 未初始化        │  可能返回新地址!
  └───────────────┴───────────────┘
```

### 原地扩容 vs 异地搬迁（核心概念）

realloc 的行为取决于当前内存块后面是否有空闲空间：

```
原地扩容 [in-place]:
  ┌── 原 4 字节 ─┬── 新增 4 字节 ─┐
  │ a  b  c  d   │  ?  ?  ?  ?    │
  └──────────────┴────────────────┘
  ↑ p (= tmp) — 返回原指针 ✓

异地搬迁 [moved]:
  旧内存(已释放):                 新内存:
  ┌── 原 4 字节 ─┐                ┌── 4 字节 ──┬── 新增 ──┐
  │ a  b  c  d   │  → memcpy →    │ a  b  c  d  │ ?  ?  ?  ?│
  └──────────────┘                └─────────────┴──────────┘
  ↑ p (已释放! 不能再用!)           ↑ tmp (新指针, ≠ p)
```

### 致命陷阱：`ptr = realloc(ptr, size)` 为什么是内存泄漏？

```c
/* ❌ 致命写法 */
arr = realloc(arr, new_capacity * sizeof(int));
```

**三步灾难**：

```
1. realloc 尝试扩容 arr → 假设失败（如内存不足）
2. realloc 返回 NULL
3. arr = NULL → 原内存的指针彻底丢失！
4. 无法 free(arr) → 原内存泄漏
5. 后续访问 arr[i] → 段错误
```

**正确写法**：

```c
/* ✅ 安全写法 */
int *tmp = realloc(arr, new_capacity * sizeof(int));
if (tmp) {
    arr = tmp;          // 成功 → 更新指针
} else {
    /* 失败: arr 仍指向原内存, 有效!
       可以选择: 继续用原大小、报错退出、或 free(arr) 清理 */
    free(arr);
    return 1;
}
```

### 扩容策略：为什么 ×2 而不是 +1？

| 策略 | 插入 n 个元素总拷贝次数 | 复杂度 |
|------|----------------------|--------|
| `cap += 1` | 1+2+3+...+(n-1) = n²/2 | O(n²) |
| `cap *= 2` | 1+2+4+...+n/2 ≈ 2n | O(n) 均摊 |
| `cap *= 1.5` | 略多但省空间 | O(n) 均摊 |

```
n=16, cap×2 的扩容过程:
  cap 2→4: 拷贝 2 元素
  cap 4→8: 拷贝 4 元素
  cap 8→16: 拷贝 8 元素
  总拷贝: 2+4+8=14 元素 → 近似 2n = 32, 但实际 ≈ n ✓

n=16, cap+1 的扩容过程:
  每次扩容拷贝所有已有元素: 1+2+...+15=120 → n²/2 = 128 ✗
```

这就是 C++ `std::vector`、Java `ArrayList` 都用加倍策略的原因——均摊 O(1) 的插入时间。

### 完整逐步跟踪：输入 "1 2 3 4 5"

```
初始: malloc(2*sizeof(int)), cap=2
      printf("initial: %p (cap=%d)\n", (void*)arr, capacity)

val=1: size=0 < cap=2 → arr[0]=1, size=1
val=2: size=1 < cap=2 → arr[1]=2, size=2

═══════════════════════════════════════════════
val=3: size=2 >= cap=2 → 扩容!
═══════════════════════════════════════════════
  uintptr_t old_addr = (uintptr_t)arr;
  old_cap = 2, cap = 4
  tmp = realloc(arr, 4*sizeof(int))
  
  if (tmp) {
      moved = ((uintptr_t)tmp != old_addr);
      printf("realloc: 0x%lx -> 0x%lx (cap 2 -> 4) %s\n",
             old_addr, (uintptr_t)tmp,
             moved ? "[moved]" : "[in-place]");
      arr = tmp; expansions++;
  }
  arr[2]=3, size=3

val=4: size=3 < cap=4 → arr[3]=4, size=4

═══════════════════════════════════════════════
val=5: size=4 >= cap=4 → 扩容!
═══════════════════════════════════════════════
  old_cap = 4, cap = 8
  (同上流程, expansions++ 到 2)
  arr[4]=5, size=5

最终: expansions: 2, final cap: 8, values: 1 2 3 4 5 ✓
```

### 打印地址的技术细节

```c
#include <stdint.h>   /* uintptr_t */

uintptr_t old_addr = (uintptr_t)arr;     /* 保存旧地址的数值 */
int *tmp = realloc(arr, capacity * sizeof(int));

if (tmp) {
    int moved = ((uintptr_t)tmp != old_addr);  /* 比较数值, 避开 GCC 警告 */
    printf("realloc: 0x%lx -> 0x%lx (cap %d -> %d) %s\n",
           (unsigned long)old_addr, (unsigned long)(uintptr_t)tmp,
           old_cap, capacity, moved ? "[moved]" : "[in-place]");
    arr = tmp;
}
```

**为什么不用 `(tmp != arr)`？** GCC 14+ 的 `-Wuse-after-free` 会对 realloc 后的指针比较产生误报。用 `uintptr_t` 保存为整数值再做比较，彻底消除误报。

### realloc 的其他特例

| 调用形式 | 等价于 | 说明 |
|---------|--------|------|
| `realloc(NULL, size)` | `malloc(size)` | 统一分配/扩容逻辑 |
| `realloc(ptr, 0)` | `free(ptr)` | C 标准说"实现定义"，不推荐依赖 |

### 常见错误

| 错误 | 后果 | 正确 |
|------|------|---------|
| `arr = realloc(arr, size)` | 失败时 arr=NULL，原内存泄漏 | `tmp = realloc(...); if (tmp) arr = tmp;` |
| 扩容后忘记更新 capacity | 后续越界写入 | `capacity *= 2;` |
| realloc 返回 NULL 不做处理 | 使用已释放内存 | `if (!tmp) { free(arr); return 1; }` |
| size 忘 sizeof | 分配字节数错误 | `capacity * sizeof(int)` |
| 在 realloc 之后用旧指针 | 可能悬空（moved 时） | 永远用新指针 |

### 重要知识点

- `realloc` 可能返回原指针（原地）或新指针（异地）——绝不要假设返回值
- `arr = realloc(arr, size)` 是隐蔽的内存泄漏陷阱
- 加倍扩容策略：均摊 O(1) 插入 vs O(n²) 的 +1 策略
- `uintptr_t` 保存地址数值是 GCC 14 兼容的最佳实践
- C++ `std::vector`、Java `ArrayList` 都使用类似策略

### 课堂讨论

1. **realloc(NULL, size) 等价什么？** — `malloc(size)`。这种统一性让代码更简洁——初始分配和后续扩容可用同一逻辑。

2. **为什么 C++ 有 new/delete 还要学 realloc？** — C++ 的 new 调构造函数。realloc 只搬运内存不调构造，不能用于有构造/析构的 C++ 对象。但理解 realloc 有助于理解 `std::vector` 的扩容原理。

3. **容量加倍的最优倍率？** — 2 倍最简单高效。1.5 倍空间利用率更好但拷贝次数稍多。Facebook 的 `folly::FBVector` 用 1.5 倍。对于教学，2 倍最直观。

4. **realloc 后旧指针还能用吗？** — 不能。即使原地扩容（值相同），也应视为失效。这是防御性编程的良好习惯。

5. **如果 realloc 总是失败怎么办？** — 保留原指针，可以选择：a) 减小扩容幅度，b) 释放旧数据并报错退出，c) 用磁盘缓存代替内存。

### 后续衔接

- Lesson 35: 环形队列——用固定数组实现动态结构
- Lesson 42: snprintf——预分配缓冲区大小
- 扩展: 实现一个简易的 `vector` 库（push_back/pop_back/at）

### 完整代码模式

```
初始分配:
  int capacity = 2;
  int *arr = malloc(capacity * sizeof(int));
  int size = 0;

扩容循环（本题需要实现的模式）:
  while (还有数据) {
    if (size >= capacity) {
      int old_cap = capacity;
      capacity *= 2;                          // 加倍策略

      uintptr_t old_addr = (uintptr_t)arr;    // 保存旧地址
      int *tmp = realloc(arr, capacity * sizeof(int));

      if (tmp) {
        int moved = ((uintptr_t)tmp != old_addr);  // 判断原地/异地
        printf("realloc: 0x%lx -> 0x%lx (cap %d -> %d) %s\n",
               (unsigned long)old_addr, (unsigned long)(uintptr_t)tmp,
               old_cap, capacity, moved ? "[moved]" : "[in-place]");
        arr = tmp;
        expansions++;
      } else {
        /* realloc 失败 — arr 仍然有效! */
        free(arr);
        return 1;
      }
    }
    arr[size++] = val;
  }
```

### 堆内存布局与 realloc

```
堆内存示意:

┌─ 已分配 ─┬─ 空闲 ──┬─ 已分配 ─┬── 空闲 ───┐
│ A (4B)   │  free    │ B (8B)   │   free     │
└──────────┴──────────┴──────────┴────────────┘

realloc(A, 8):
  后面有空闲 → 直接扩展 → 原地 [in-place] ✓
  ┌─ A (8B) ─────────────┬─ 已分配 ─┬── 空闲 ───┐
  │ 旧数据 + 新空间        │ B (8B)   │   free     │
  └──────────────────────┴──────────┴────────────┘
  返回原指针

realloc(B, 16):
  后面空闲不够 → 新地址 + memcpy + 释放旧块 → 异地 [moved]
  旧位置: ┌─ 已分配 ─┬── 空洞 ──┐
          │ A (8B)   │ B(old)   │  ← B 被释放，留下碎片
          └──────────┴──────────┘
  新位置: ... ┌─ B (16B) ──────────────────┐ ...
             │ 旧数据 + 新空间               │
             └─────────────────────────────┘
  返回新指针
```

频繁 realloc 会导致**堆碎片化**——大量不连续的小空洞。这就是为什么偶尔 `realloc` 返回新地址（异地搬迁）是正常现象。

### 参考资料

- `man realloc` — 标准库文档
- C 标准 §7.20.3.4 — realloc 规范
- Facebook `folly::FBVector` — 1.5× 扩容策略参考
- Wilson et al. (1995) "Dynamic Storage Allocation: A Survey and Critical Review"
