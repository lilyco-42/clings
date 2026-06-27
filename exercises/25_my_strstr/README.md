## Lesson 25 — 实现 my_strstr：暴力字符串匹配

### 课程任务

实现自己的 `my_strstr(haystack, needle)` 函数——在字符串 `haystack`（干草堆）中查找子串 `needle`（针）第一次出现的位置，返回指向该位置的指针；若未找到返回 `NULL`。如果 `needle` 是空字符串，返回 `haystack`。

这是标准库 `<string.h>` 中 `strstr` 的手写版本，也是后续 KMP 算法的前置基础。

### 前置知识：C 字符串与滑动窗口

#### C 字符串的内存模型

C 字符串以 `'\0'`（ASCII 值为 0）结尾：

```
"hello" 在内存中:
┌───┬───┬───┬───┬───┬───┐
│ h │ e │ l │ l │ o │ \0│
└───┴───┴───┴───┴───┴───┘
 0   1   2   3   4   5   ← 索引
strlen("hello") = 5  (不含 \0)
```

#### 滑动窗口模型

字符串匹配的核心是**滑动窗口**：一个宽度为 `m`（needle 长度）的窗口，在 haystack 上从左向右滑动。每到一个位置，比较窗口内的字符和 needle。

```
haystack = "hello world", needle = "world" (m=5)

位置 i=0:  ┌─────────────┐
           │ h e l l o   │ w o r l d    → "hello" vs "world" → ✗
           └─────────────┘

位置 i=1:    ┌─────────────┐
           h │ e l l o   w │ o r l d    → "ello " vs "world" → ✗
             └─────────────┘

... 滑动 ...

位置 i=6:              ┌─────────────┐
           h e l l o   │ w o r l d │      → "world" vs "world" → ✓ 匹配!
                       └─────────────┘
                       返回 &haystack[6]
```

**窗口的最后一个合法起点在哪里？** 答案是 `n - m`（n = strlen(haystack), m = strlen(needle)）。如果起点超过 `n-m`，窗口会超出字符串末尾。

```
haystack = "hello" (n=5), needle = "world" (m=5)

位置 i=0:  ┌─────────────┐
           │ h e l l o   │    → "hello" vs "world" → ✗
           └─────────────┘

位置 i=1: 已经是 n-m+1 = 1，窗口宽度 5 会超出 → 不用检查了
(window: h │ e l l o ? │ → 越界)
```

### 算法详解

#### 双重循环结构

```
外层循环: i 从 0 到 n-m（窗口的起始位置）
  内层循环: j 从 0 到 m-1（窗口内逐字符比较）
    若 haystack[i+j] != needle[j] → 不匹配，break
  若 j == m → 内层完整走完，全部匹配！返回 &haystack[i]
外层循环结束 → 未找到，返回 NULL
```

#### 循环变量含义图解

```
haystack: "h e l l o   w o r l d"
needle:   "w o r l d"

当 i=6 时，进入内层比较:

        i=6
        ↓
        h e l l o   w o r l d
        ┌─┬─┬─┬─┬─┐
j=0:    │w│o│r│l│d│   比较 haystack[6+0]=w  vs needle[0]=w → 匹配 ✓
        └─┴─┴─┴─┴─┘
j=1:    h e l l o   w o r l d
          ┌─┬─┬─┬─┬─┐
          │w│o│r│l│d│ 比较 haystack[6+1]=o  vs needle[1]=o → 匹配 ✓
          └─┴─┴─┴─┴─┘
... 
j=4:    h e l l o   w o r l d
                          ┌─┐
                          │d│ 比较 haystack[6+4]=d  vs needle[4]=d → 匹配 ✓
                          └─┘

j==5==m → 全部匹配！返回 &haystack[6] = "world"
```

### 完整示例：逐步跟踪表

**示例 1**：haystack="hello world", needle="world" → 应返回 "world"

| i | 窗口内容 | j 遍历过程 | 结果 |
|---|---------|-----------|------|
| 0 | "hello" | j=0:'h'≠'w'→break | ✗ |
| 1 | "ello " | j=0:'e'≠'w'→break | ✗ |
| 2 | "llo w" | j=0:'l'≠'w'→break | ✗ |
| 3 | "lo wo" | j=0:'l'≠'w'→break | ✗ |
| 4 | "o wor" | j=0:'o'≠'w'→break | ✗ |
| 5 | " worl" | j=0:' '≠'w'→break | ✗ |
| 6 | "world" | j=0✓ j=1✓ j=2✓ j=3✓ j=4✓→j==5 | ✓ 返回 &haystack[6] |

**示例 2**：haystack="aaaa", needle="aa" → 应返回 "aaaa"（首次匹配在位置 0）

| i | 窗口内容 | j 遍历过程 | 结果 |
|---|---------|-----------|------|
| 0 | "aa" | j=0:'a'='a'✓ j=1:'a'='a'✓ → j==2 | ✓ 返回 &haystack[0] |

### 边界条件分析

| 边界情况 | 预期行为 | 为什么 |
|---------|---------|--------|
| needle 为空串 `""` | 返回 haystack | C 标准规定：空串在任何位置都能"匹配" |
| n < m（haystack 比 needle 短） | 返回 NULL | n-m 为负，循环不会执行 |
| n == m（等长） | 仅检查 i=0 | 窗口只有 1 个可能的起点 |
| needle 出现在末尾 | 正确返回 | `i <= n-m` 的等号保证不遗漏末尾 |
| needle 出现在开头 | 正确返回 | i=0 时即匹配 |
| 有重复前缀 | 返回第一次出现 | 窗口从左到右扫描，天然保证首次匹配 |

### 复杂度分析

| 指标 | 值 | 说明 |
|------|-----|------|
| 最坏时间复杂度 | O(n×m) | haystack 每个位置都几乎比较完 |
| 最好时间复杂度 | O(n) | 每次第一个字符就不匹配 |
| 空间复杂度 | O(1) | 不需要额外空间 |
| 最坏场景 | haystack="AAAAA...A", needle="AAA...AB" | 每次都匹配到最后一个字符才失败 |

**最坏场景演示**：

```
haystack: "AAAAAB" (n=6)
needle:   "AAAAB"  (m=5)

i=0: AAAAA? → 比到第5个字符发现 B≠A → break (5次比较)
i=1: AAAA?B → 比到第4个字符发现 A≠B → break (4次比较)
总比较次数 ≈ n×m/2，接近 O(n×m)
```

### 重要知识点

- 双重循环的滑动窗口模型：外层控制窗口位置，内层逐字符验证
- `i <= n - m` 的等号不能丢：窗口起点恰好等于 n-m 时，窗口右边界 = n，是最末尾的合法位置
- `const` 修饰符：`const char *haystack` 表示函数承诺不修改字符串内容
- 空串处理：`*needle == '\0'` 时返回 haystack——这是 C 标准的行为
- 指针返回值：找到时返回子串起始地址，未找到返回 NULL

### 课堂讨论

1. **为什么外层循环条件是 `i <= n-m` 而不是 `i < n`？** — 用 `i < n` 会导致窗口超出字符串末尾，访问越界内存。`i <= n-m` 确保窗口右边界不超过 n。

2. **如果 needle 长度为 0（空串），应该返回什么？** — C 标准（man strstr）规定返回 haystack。意味着空串被认为在任何位置都能匹配。`*needle == '\0'` 检查即是此用途。

3. **暴力匹配能处理 100 万字符的文本吗？** — 取决于 needle 的长度和内容。如果 needle 很短且匹配位置靠前，很快；如果 needle 长且有很多"几乎匹配"的位置，会很慢。这就是为什么需要 KMP 算法。

4. **这个算法是大小写敏感的吗？** — 是。`'H' != 'h'`。如果需要大小写不敏感匹配，要用 `tolower()` 转换后再比较。

### 后续衔接

本课的暴力匹配为 Lesson 44 的 **KMP 算法**打下基础。KMP 通过预处理 needle 的前缀表（next 数组），将时间复杂度从 O(n×m) 降到 O(n+m)。理解暴力匹配中 j 的回退行为，才能体会 KMP "只前进不后退"的精妙。

### 参考资料

- `man strstr` — 标准库文档
- 《算法导论》第 32 章 字符串匹配
