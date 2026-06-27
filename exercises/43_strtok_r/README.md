## Lesson 43 — strtok_r 线程安全字符串切分

### 课程任务

使用 `strtok_r` 将命令行字符串按空格和制表符切分为独立参数，存入 `char *argv[]` 数组（模拟 `argc/argv`），为后续 MyBash 项目热身。需要正确处理：

1. **空格分隔** — `"ls -l -a /tmp"` 切分为 4 个 token
2. **连续空格** — `"echo   hello"` 中连续空格不产生空 token（自动跳过）
3. **制表符分隔** — `"gcc\t-Wall\tmain.c"` 中 tab 同样是分隔符

```
验证用例:
  输入 "ls -l -a /tmp\n"    → argc: 4, argv[0]: ls ...
  输入 "echo   hello\n"     → argc: 2 (连续空格不产生空token)
  输入 "gcc\t-Wall\tmain.c\n" → argc: 3 (tab也是分隔符)
```

### 前置知识：strtok_r 的签名与内存行为

```c
char *strtok_r(char *str, const char *delim, char **saveptr);
```

**三个参数的含义**：

| 参数 | 首次调用 | 后续调用 | 说明 |
|------|---------|---------|------|
| str | 待切分的字符串 | NULL | NULL 表示继续上次的切分 |
| delim | 分隔符字符集合 | 同上 | 每个字符都是独立的分隔符 |
| saveptr | `char *sp;` 传 `&sp` | 同上 | 保存切分上下文 |

**关键行为：原地修改**。`strtok_r` 找到分隔符后将其替换为 `\0`，在原字符串上直接"截断"子串。这意味着：

- 传入的字符串**必须可修改**（栈/堆上的数组，不能是字符串字面量）
- 所有 token 指针指向原字符串内部——不需要 `malloc` 新空间
- 原字符串被破坏——切分后不能再当完整字符串使用

### 原地修改的详细图解

```
原始字符串（可修改的 char 数组）:
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│ l │ s │   │ - │ l │   │ - │ a │   │ / │ t │ m │ p │ \0│
└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘

strtok_r 处理后（分隔符被替换为 \0）:
┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
│ l │ s │\0 │ - │ l │\0 │ - │ a │\0 │ / │ t │ m │ p │ \0│
└───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
  ↑           ↑           ↑           ↑
argv[0]="ls" argv[1]="-l" argv[2]="-a" argv[3]="/tmp"

所有 argv[i] 都指向原 line 内的片段——不需要额外内存分配!
```

### 分隔符字符串详解

`delim` 参数是**字符集合**，不是子串：

```
delim = " \t"
         ││
         │└─ \t: 制表符 (ASCII 0x09)
         └── ' ': 空格 (ASCII 0x20)

含义: 空格或制表符中任意一个字符出现，都视为分隔符
```

| delim | 匹配的分隔符 | 示例输入 | 切分结果 |
|-------|------------|---------|---------|
| `" "` | 仅空格 | `"a b c"` | a, b, c |
| `"\t"` | 仅 tab | `"a\tb\tc"` | a, b, c |
| `" \t"` | 空格或 tab | `"a b\tc"` | a, b, c |
| `",;"` | 逗号或分号 | `"a,b;c"` | a, b, c |

### 算法详解：strtok_r 的工作流程

#### 首次调用 `strtok_r(str, delim, &saveptr)`

```
输入: "ls  -l  -a  /tmp\0"

步骤 1: 跳过前导分隔符（如果有的话）
         "ls  -l  -a  /tmp\0" — 无前导分隔符

步骤 2: 从当前位置开始扫描，寻找分隔符
         "ls  -l  -a  /tmp\0"
           ↑     找到空格!

步骤 3: 将分隔符替换为 \0
         "ls\0 -l  -a  /tmp\0"
           ↑
         返回指向 "ls" 的指针

步骤 4: saveptr 指向 \0 之后
         "ls\0 -l  -a  /tmp\0"
              ↑
            saveptr 指在这里
```

#### 后续调用 `strtok_r(NULL, delim, &saveptr)`

```
从 saveptr 继续:

步骤 1: 跳过连续分隔符
         "ls\0 -l  -a  /tmp\0"
               ↑  ↑  (saveptr) 跳过空格

步骤 2: 扫描寻找下一个分隔符
             "-l  -a  /tmp\0"
                ↑  找到空格!

步骤 3: 替换为 \0, 返回 "-l", 更新 saveptr
```

### 逐步跟踪：解析 "echo   hello"

```
原始: "echo   hello\0"     (echo 后有两个空格)

首次调用 strtok_r(line, " \t", &saveptr):
  跳过前导: 无
  扫描:     遇到第一个空格(echo后)
  替换:     "echo\0  hello\0"
  返回:     argv[0] = "echo"
  saveptr:  指向第一个空格后的第二个空格

后续调用 strtok_r(NULL, " \t", &saveptr):
  跳过连续: 跳过第二个空格
  扫描:     遇到 \0 (hello后)
  替换:     "echo\0  hello\0"
  返回:     argv[1] = "hello"
  saveptr:  指向末尾 \0

后续调用:
  saveptr 指向 \0 → 无更多 token → 返回 NULL

输出: argc=2, argv[0]="echo", argv[1]="hello" ✓
```

### strtok vs strtok_r：线程安全

```c
/* strtok 内部实现（简化） */
char *strtok(char *str, const char *delim) {
    static char *save;          // ← 静态变量! 全局唯一!
    if (str) save = str;
    // ... 使用 save 做切分 ...
}

/* strtok_r 内部实现（简化） */
char *strtok_r(char *str, const char *delim, char **saveptr) {
    if (str) *saveptr = str;    // ← 调用者提供的指针! 每线程独立!
    // ... 使用 *saveptr 做切分 ...
}
```

**为什么 strtok 不是线程安全的？** `static char *save` 是全局唯一变量。线程 A 正在切分字符串时，线程 B 也调用 strtok，会覆盖 save，导致线程 A 的状态丢失。

**为什么 strtok_r 是线程安全的？** 每个调用者提供自己的 `saveptr`，互不影响。

### 常见错误

| 错误 | 后果 | 正确 |
|------|------|---------|
| 传入字符串常量 | 段错误（尝试修改只读内存） | 传入可修改的 char[] |
| 分隔符写成 `" \t "` 有尾部空格 | 空格也被当作分隔符字符（不影响结果但非预期） | `" \t"` |
| 忘判 token 是否为 NULL | 段错误 | `while(token){...}` |
| 忘记首次调用传字符串、后续传 NULL | 行为异常 | 首次传 str，后续 NULL |
| 后续调用仍传原字符串 | 无限循环切出同一个 token | `strtok_r(NULL, delim, &sp)` |

### 重要知识点

- `strtok_r` 原地修改原字符串——分隔符被替换为 `\0`
- `saveptr` 是 strtok_r 线程安全的保证——每个调用者提供自己的上下文
- 分隔符参数是字符集合，不是子串
- 自动跳过连续分隔符——不会产生空 token
- token 指针指向原字符串内部——不需要额外内存分配

### 课堂讨论

1. **strtok 为什么不是线程安全的？** — 用 `static char *` 保存切分位置。多线程同时调用会互相覆盖状态。

2. **如果把字符串常量 `"hello world"` 传给 strtok_r 会怎样？** — 段错误。字符串常量存储在只读数据段，strtok_r 尝试写 `\0` 时会触发段错误。

3. **strtok_r 和手动遍历字符有什么优劣？** — strtok_r 代码简洁，自动处理连续分隔符；手动遍历更灵活（如需要保留引号内的空格）。

4. **为什么 argv[] 不需要 malloc？** — strtok_r 原地修改原字符串，所有 token 指针都指向原数组内部。这是 strtok_r 的核心设计——高效且简洁。

5. **如何切分 CSV 行（逗号分隔，引号内有逗号）？** — strtok_r 不能处理引号内的保护字符。需要手动状态机（跟踪"是否在引号内"）。这是 strtok_r 的局限。

### 后续衔接

- Lesson 32: MyBash 项目——用 argv[] 实现命令分发
- Lesson 44: qsort——另一个标准库 API 深度使用
- Unit 3: 词法分析器——更复杂的字符串切分（状态机）

### 参考资料

- `man strtok_r` — 标准库文档
- POSIX.1-2001 — strtok_r 的正式规范
