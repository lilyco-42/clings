## Lesson 61 — 缓冲区溢出安全分析实验【安全】

### 课程任务

给定 3 段 C 代码片段，完成"缓冲区溢出安全分析"——不执行实际溢出攻击，而是作为安全分析师，系统地分析每段代码的内存布局、溢出风险和防护方案。

**三片段清单**：

| 片段 | 代码                                         | 风险等级 |
| ---- | -------------------------------------------- | -------- |
| 1    | `gets(buf)` — `buf[16]`                      | CRITICAL |
| 2    | `strcpy(buf, src)` — `buf[32]`               | HIGH     |
| 3    | `fgets(buf, sizeof(buf), stdin)` — `buf[16]` | SAFE     |

**学生任务**：

1. 为每段代码画出 ASCII 栈帧布局图（标注地址偏移）
2. 计算局部变量 buf 到返回地址的字节偏移量
3. 判断是否存在溢出风险并解释原因
4. 深入讲解 Stack Canary（金丝雀）防护原理
5. 输出防护机制对比表（Canary / ASLR / DEP / RELRO / PIE / FORTIFY）
6. 给出综合风险评估和安全编程准则

输出是一个固定格式的安全分析报告，与 `expected_output.txt` 精确匹配。

### 为什么缓冲区溢出是 C 语言的"原罪"？

1988 年 11 月 2 日，Robert T. Morris Jr. 释放了互联网上第一个蠕虫——Morris Worm。它利用 `gets()` 函数在 `fingerd` 服务中的缓冲区溢出漏洞，在 24 小时内感染了约 6000 台 Unix 主机（当时互联网的 ~10%）。此后 30 多年，缓冲区溢出始终位列 CWE Top 25 最危险软件漏洞。

```
1988 Morris Worm 攻击链:
  fingerd 服务 (端口 79)
      │
      ▼
  gets(buf)  ← buf 只有 512 字节
      │
      ▼
  输入 537 字节 → 溢出覆盖返回地址
      │
      ▼
  跳转到 shellcode (栈上的恶意代码)
      │
      ▼
  获得 root shell → 传播自身
```

**本题定位**：不是 CTF（Capture The Flag）的漏洞利用教学，而是**安全分析能力培养**——理解漏洞产生的原因，才能在编码时从源头避免。

### 前置知识：栈帧 (Stack Frame) 结构

#### 函数调用的栈操作全景

每当一个函数被调用，CPU 执行以下操作：

```
调用者 (caller) 视角:
  push 参数        ← 将参数压栈 (x86-64 前6个用寄存器)
  call function    ← 将返回地址 (RIP) 压栈，跳转到函数

被调用者 (callee) 视角:
  push rbp         ← 保存调用者的栈帧基址
  mov  rbp, rsp    ← 建立自己的栈帧基址
  sub  rsp, N      ← 为局部变量分配 N 字节空间
  ... 函数体 ...
  leave            ← mov rsp, rbp; pop rbp (恢复调用者栈帧)
  ret              ← pop rip (弹出返回地址并跳转)
```

#### x86-64 典型栈帧布局

```
      高地址 (栈底方向，地址值更大)
      ┌──────────────────────┐
      │  调用者的栈帧         │  ← 调用者的局部变量等
      ├──────────────────────┤
      │  函数参数 (第7个起)   │  ← x86-64 前6个参数用寄存器
      ├──────────────────────┤
      │  返回地址 (8 bytes)   │  ← call 指令压入的 RIP
      ├──────────────────────┤
      │  保存的 rbp (8 bytes) │  ← 调用者的栈帧基址
      ├──────────────────────┤ ← rbp (当前函数栈帧基址)
      │  canary (8 bytes)     │  ← -fstack-protector 插入的哨兵
      ├──────────────────────┤
      │  [栈对齐填充]          │  ← x86-64 ABI 要求 16 字节对齐
      ├──────────────────────┤
      │  局部变量 / 数组      │  ← char buf[N] 从这里开始
      ├──────────────────────┤
      │  callee-saved 寄存器  │  ← rbx, r12-r15 (如果使用)
      ├──────────────────────┤
      │  可能的 red zone      │  ← x86-64 在 rsp-128 以下的 128 字节
      └──────────────────────┘
      低地址 (栈顶方向) ← rsp
```

#### 关键事实：溢出方向

```
栈增长方向:  高地址 → 低地址  (push 操作递减 rsp)
数组增长方向: 低地址 → 高地址  (buf[i] 的地址 = buf + i)

因此:
  buf[0]   在最低地址
  buf[N-1] 在较高地址
  向 buf[N] 写入 → 向高地址越界 → 覆盖 canary → 覆盖 rbp → 覆盖返回地址

示意:
  低地址                    buf                        canary/rbp/ret   高地址
  ─────────────────────────────────────────────────────────────────────────→
  [buf[0]][buf[1]]...[buf[N-1]][超出部分覆盖canary][覆盖rbp][覆盖返回地址]
  写入起点 ←──────────────────── 溢出方向 ────────────────────────────→
```

### 代码片段深度分析

#### 片段 1：gets(buf) — CRITICAL

```c
void vulnerable_gets(void) {
    char buf[16];
    gets(buf);  /* 危险！gets 不检查边界 */
}
```

**栈帧布局**（x86-64，无 canary）：

```
      地址偏移       内容                说明
      ─────────────────────────────────────────────────
      +24           ┌──────────────┐
                    │  返回地址     │ ← gets 溢出可覆盖！
      +16           ├──────────────┤
                    │  保存的 rbp   │
      +8            ├──────────────┤
                    │  [栈对齐填充]  │  (8 bytes, 对齐到 16 字节)
      +0            ├──────────────┤
                    │  buf[0..15]   │ ← gets() 从这里开始写入
                    └──────────────┘
```

**偏移量计算**：

```
buf[0] 到返回地址的距离 = 16 (buf) + 8 (对齐填充) + 8 (rbp) = 32 字节

注：若编译器紧密排列（无对齐填充），则为 24 字节。
    实际偏移取决于编译器版本和优化级别。
    本题使用 32 字节作为教学示例（包含对齐）。
```

**溢出机制**：

```
正常输入 "hello" (5 字节 + \0):
  buf: [h][e][l][l][o][\0]... ← 安全，未触及返回地址

恶意输入 'A' × 32 + 恶意地址 (8 字节):
  buf: [A][A]...[A][A][A][A]...[A][恶意地址]
       └── buf[0..15] ──┘└─填充─┘└─rbp─┘└─返回地址─┘
  结果: 函数 "返回" 到攻击者指定的地址 → 控制流劫持
```

**gets() 为什么如此危险**：

1. **零边界检查**：`gets` 只有一个参数（缓冲区指针），不知道缓冲区大小
2. **任意长度输入**：从 stdin 读取直到换行符，可以是任意长度
3. **直接可攻击**：攻击者通过 stdin 直接控制溢出内容
4. **已被标准移除**：C11 标准正式移除 `gets()`，但遗留代码中仍大量存在

#### 片段 2：strcpy(buf, src) — HIGH

```c
void vulnerable_strcpy(char *src) {
    char buf[32];
    strcpy(buf, src);  /* 危险！strcpy 不检查边界 */
}
```

**栈帧布局**（x86-64，无 canary）：

```
      地址偏移       内容                说明
      ─────────────────────────────────────────────────
      +40           ┌──────────────┐
                    │  返回地址     │ ← strcpy 溢出可覆盖！
      +32           ├──────────────┤
                    │  保存的 rbp   │
      +0            ├──────────────┤
                    │  buf[0..31]   │ ← strcpy() 从这里开始写入
                    └──────────────┘
```

**偏移量计算**：

```
buf[0] 到返回地址的距离 = 32 (buf) + 8 (rbp) = 40 字节
(32 字节已经 16 字节对齐，无需额外填充)
```

**gets vs strcpy 溢出对比**：

| 维度       | gets()                | strcpy()                          |
| ---------- | --------------------- | --------------------------------- |
| 输入源     | stdin（标准输入）     | 字符串参数（内存中的 src）        |
| 终止条件   | 换行符 `'\n'`         | 空字符 `'\0'`                     |
| 攻击难度   | 低（直接 stdin 输入） | 中（需控制 src 内容和长度）       |
| `\0` 限制  | 无（可包含任意字节）  | 有（`\0` 终止拷贝，限制 payload） |
| 典型场景   | 网络服务、命令行工具  | 字符串处理、配置文件解析          |
| C 标准状态 | C11 移除              | 仍保留（但有安全替代）            |

**strcpy 的特殊风险**：

```
如果 src 指向一个超长字符串（无 \0 或很晚才有 \0）：
  strcpy 将持续拷贝直到遇到 \0
  → 覆盖 buf[32..39] (rbp)
  → 覆盖 buf[40..47] (返回地址)
  → 继续覆盖调用者的栈帧...

注意：strcpy 的 \0 终止是一把双刃剑：
  - 限制了 payload 中不能包含 \0 字节
  - 但也不能防止溢出本身
```

#### 片段 3：fgets(buf, sizeof(buf), stdin) — SAFE

```c
void safe_fgets(void) {
    char buf[16];
    fgets(buf, sizeof(buf), stdin);  /* 安全！限定了最大长度 */
}
```

**栈帧布局**（同片段 1，但行为完全不同）：

```
      地址偏移       内容                说明
      ─────────────────────────────────────────────────
      +24           ┌──────────────┐
                    │  返回地址     │ ← fgets 无法到达！
      +16           ├──────────────┤
                    │  保存的 rbp   │ ← fgets 无法到达！
      +8            ├──────────────┤
                    │  [栈对齐填充]  │ ← fgets 无法到达！
      +0            ├──────────────┤
                    │  buf[0..15]   │ ← fgets 最多写 15+1 字节
                    └──────────────┘
```

**为什么安全**：

```
fgets(buf, 16, stdin) 的行为：
  1. 最多读取 15 个字符（第 2 个参数 - 1）
  2. 第 16 个字节固定写入 '\0'
  3. 遇到换行符 '\n' 也停止
  4. 遇到 EOF 也停止

因此：最大写入范围 = buf[0..15]，共 16 字节
     返回地址在 +24 偏移处，完全无法触及
```

**fgets 的正确使用姿势**：

```c
/* ✅ 正确：使用 sizeof 自动适配缓冲区大小 */
char buf[16];
fgets(buf, sizeof(buf), stdin);

/* ✅ 也正确：显式指定（但需确保与声明一致） */
fgets(buf, 16, stdin);

/* ❌ 错误：传入错误的 size（大于实际缓冲区） */
char buf[16];
fgets(buf, 256, stdin);  /* 缓冲区只有 16 字节，却告诉 fgets 有 256！ */

/* ❌ 错误：忘记 sizeof 是字节数 */
wchar_t wbuf[16];
fgets((char*)wbuf, sizeof(wbuf), stdin);  /* 32 字节！可能超出 wbuf 实际大小 */
```

### Stack Canary（栈金丝雀）防护原理

#### 名称由来：煤矿中的金丝雀

```
19世纪的煤矿:
  ┌─────────────────────────────────┐
  │ 矿工带着金丝雀笼子下矿           │
  │                                 │
  │ 金丝雀对一氧化碳 (CO) 极度敏感   │
  │ CO 浓度升高 → 金丝雀先死         │
  │ → 矿工看到金丝雀死亡 → 立即撤离  │
  │                                 │
  │ 金丝雀 = 生物传感器 / 早期预警   │
  └─────────────────────────────────┘

计算机中的 Stack Canary:
  ┌─────────────────────────────────┐
  │ 编译器在栈帧中放置一个随机值      │
  │                                 │
  │ 溢出覆盖 → canary 值被改变       │
  │ → 函数返回前检测到变化           │
  │ → 立即终止程序 (__stack_chk_fail)│
  │                                 │
  │ canary = 栈溢出检测器 / 哨兵     │
  └─────────────────────────────────┘
```

#### 带 Canary 的栈帧布局

```
      地址偏移       内容                说明
      ─────────────────────────────────────────────
      +32           ┌──────────────┐
                    │  返回地址     │ ← 攻击者的最终目标
      +24           ├──────────────┤
                    │  保存的 rbp   │
      +16           ├──────────────┤
                    │  CANARY ★     │ ← 哨兵值 (8 bytes)
      +8            ├──────────────┤
                    │  [栈对齐填充]  │
      +0            ├──────────────┤
                    │  buf[0..15]   │ ← 溢出从这里开始
                    └──────────────┘

溢出路径:
  buf[0] → buf[15] → [对齐] → CANARY → rbp → 返回地址
  安全区域 ────────→ 触发警报！→ 程序终止
```

#### Canary 完整工作流程

```
╔══════════════════════════════════════════════════════╗
║                 Canary 生命周期                       ║
╠══════════════════════════════════════════════════════╣
║                                                      ║
║  函数入口 (prologue):                                ║
║    1. 从 TLS 读取随机 canary 值                      ║
║       x86-64: mov rax, QWORD PTR fs:0x28            ║
║       x86:   mov eax, DWORD PTR gs:0x14             ║
║    2. 将 canary 写入栈帧 (通常在 rbp-8)              ║
║       mov QWORD PTR [rbp-8], rax                    ║
║                                                      ║
║  函数体执行:                                         ║
║    ... 正常代码 / 可能的溢出 ...                      ║
║                                                      ║
║  函数返回前 (epilogue):                              ║
║    3. 从栈帧读取 canary 值                           ║
║       mov rax, QWORD PTR [rbp-8]                    ║
║    4. 与 TLS 原始值异或比较                          ║
║       xor rax, QWORD PTR fs:0x28                    ║
║    5. 若结果为零 (相同) → je .L_ok → ret            ║
║    6. 若结果非零 (不同) → call __stack_chk_fail     ║
║       → 打印错误信息 → 程序 abort()                  ║
║                                                      ║
╚══════════════════════════════════════════════════════╝
```

#### Canary 的巧妙设计

**1. 最低字节为 `\0`（NULL 字节）**：

```
canary 值示例: 0x00 7f 3a 91 4c 2e b8 d5
               ↑
               \0 字节

为什么这样设计？
  大多数字符串函数 (strcpy, gets, sprintf 等) 遇到 \0 停止拷贝。
  如果攻击者想通过字符串溢出来覆盖 canary：
    "AAAA...\x00\x7f\x3a..." ← 在 \0 处 strcpy 停止！

  攻击者必须精确知道 canary 值并原样写入，
  这大大增加了攻击难度。
```

**2. 随机值（每次运行不同）**：

```
程序启动时:
  canary 由内核的随机数生成器 (getrandom/urandom) 初始化
  存储在 TLS (Thread-Local Storage) 中

每次运行:
  $ ./vuln    → canary = 0x00a3f91c...
  $ ./vuln    → canary = 0x007b2e4d...  (完全不同！)

攻击者无法硬编码 canary 值。
```

**3. 存储在 TLS 中**：

```
TLS (Thread-Local Storage):
  - 每个线程独立的存储区域
  - 不能通过普通内存访问直接读取
  - x86-64: fs 段寄存器指向 TLS
  - 需要内存泄露漏洞才能读取
```

#### Canary 的局限性

| 局限性         | 详细说明                                                                                     | 利用场景                       |
| -------------- | -------------------------------------------------------------------------------------------- | ------------------------------ |
| 信息泄露       | 若攻击者能先读取 canary 值（如通过 format string 漏洞），则可精确覆盖                        | `printf("%s", buf)` 泄露栈内容 |
| 非连续写入     | Canary 只能检测从低地址向高地址的顺序覆盖。直接写入返回地址（如任意地址写漏洞）不触发 canary | `*(int*)(buf+40) = evil_addr`  |
| Fork 暴力破解  | `fork()` 后子进程继承父进程的 canary 值。攻击者可逐字节尝试，每次崩溃只杀死子进程            | 网络服务器 fork 模型           |
| 不保护其他目标 | 覆盖函数指针、数据指针、GOT 表、或修改关键局部变量（如 `is_admin` flag）可能绕过 canary      | 逻辑漏洞利用                   |

**绕过 Canary 的 4 种经典方法**：

```
方法 A: 信息泄露 + 精确覆盖
  Step 1: 利用格式化字符串漏洞泄露 canary 值
  Step 2: 构造 payload = 填充 + canary原值 + rbp + 恶意返回地址
  Step 3: 触发溢出 → canary 检测通过 → 劫持返回地址

方法 B: 改写 GOT 表（不碰 canary）
  Step 1: 溢出 buf，但不覆盖返回地址
  Step 2: 覆盖 buf 后面的函数指针 → 指向 system()
  Step 3: 后续代码调用该函数指针 → 执行 system()

方法 C: 覆盖局部变量
  void login() {
      char buf[16];
      int is_admin = 0;     ← 在 buf 之后（更高地址）
      gets(buf);            ← 溢出覆盖 is_admin 为 1
      if (is_admin) { ... } ← 绕过认证！
  }

方法 D: 逐字节暴力破解（fork 服务器）
  for (byte_pos = 1; byte_pos < 8; byte_pos++) {
      for (guess = 0; guess < 256; guess++) {
          发送 payload: 填充 + [已破解字节] + guess
          if (子进程没有崩溃) → 该字节正确！
      }
  }
  // 平均需要 8 × 128 = 1024 次尝试
```

### 防护机制全景对比

#### 纵深防御体系 (Defense in Depth)

```
现代操作系统采用多层防护，每层针对不同攻击阶段：

    攻击者
      │
      ▼
  ┌──────────┐
  │  Canary  │  第1层: 检测栈溢出
  └────┬─────┘       (阻止简单的栈粉碎)
       │ 绕过
       ▼
  ┌──────────┐
  │   ASLR   │  第2层: 隐藏地址布局
  └────┬─────┘       (使 ROP/JOP 难以定位 gadget)
       │ 绕过
       ▼
  ┌──────────┐
  │ DEP/NX   │  第3层: 阻止代码注入
  └────┬─────┘       (栈/堆不可执行)
       │ 绕过
       ▼
  ┌──────────┐
  │  RELRO   │  第4层: 保护 GOT 表
  └────┬─────┘       (阻止 GOT 覆写)
       │ 绕过
       ▼
     目标
```

#### 完整对比表

| 防护机制           | 全称                                   | 防护目标                 | 工作原理                                            | 启用方式                                         | 局限性                                             |
| ------------------ | -------------------------------------- | ------------------------ | --------------------------------------------------- | ------------------------------------------------ | -------------------------------------------------- |
| **Stack Canary**   | Stack Smashing Protector               | 栈缓冲区溢出检测         | 返回地址前放置随机哨兵值，返回前校验                | `-fstack-protector-strong`                       | 信息泄露可绕过；不保护非栈目标                     |
| **ASLR**           | Address Space Layout Randomization     | 代码复用攻击 (ROP/JOP)   | 随机化栈、堆、mmap、库、可执行文件的基地址          | 内核默认 (`/proc/sys/kernel/randomize_va_space`) | 信息泄露可绕过；32-bit 熵不足 (~8 bit)             |
| **DEP / NX-bit**   | Data Execution Prevention / No-eXecute | 代码注入攻击 (Shellcode) | CPU 标记数据页为不可执行；栈/堆不能同时写 + 执行    | 硬件 NX-bit + OS 支持 (`-z noexecstack`)         | ROP/JOP 可绕过（复用已有代码）；JIT 需要可执行堆   |
| **RELRO**          | Relocation Read-Only                   | GOT 表覆写攻击           | 将重定位表设为只读；Full RELRO 在启动时解析所有符号 | `-Wl,-z,relro` / `-Wl,-z,now`                    | Partial RELRO 只保护 .got；Full RELRO 增加启动时间 |
| **PIE**            | Position Independent Executable        | 固定地址攻击             | 可执行文件编译为位置无关代码，配合 ASLR 随机加载    | `-fPIE -pie`                                     | 需 ASLR 配合；轻微性能开销 (~1-3%)                 |
| **FORTIFY_SOURCE** | Compile-time Fortification             | 不安全库函数调用         | 编译时替换 `strcpy` → `__strcpy_chk`（带边界检查）  | `-D_FORTIFY_SOURCE=2`                            | 需源码重编译；不能检测所有情况；对某些模式无效     |

#### 实际编译示例

```bash
# 最小防护（危险！仅用于教学演示）
gcc -fno-stack-protector -z execstack -no-pie -o vuln vuln.c

# 标准防护（现代 Linux 默认）
gcc -fstack-protector-strong -D_FORTIFY_SOURCE=2 \
    -fPIE -pie -Wl,-z,relro -Wl,-z,now \
    -o secure secure.c

# 查看二进制启用的安全特性
checksec --file=./secure
# 输出示例:
#   RELRO:    Full RELRO
#   STACK CANARY:  Found
#   NX:       Enabled
#   PIE:      Enabled
```

### 安全编程准则

#### 危险函数及其安全替代

| 危险函数                 | 风险            | 安全替代                                | 注意事项                         |
| ------------------------ | --------------- | --------------------------------------- | -------------------------------- |
| `gets(buf)`              | 零边界检查      | `fgets(buf, sizeof(buf), stdin)`        | fgets 保留换行符，需手动去除     |
| `strcpy(dst, src)`       | 不检查 dst 大小 | `strncpy(dst, src, n-1); dst[n-1]='\0'` | strncpy 不保证 `\0` 结尾         |
| `strcat(dst, src)`       | 不检查剩余空间  | `strncat(dst, src, n-strlen(dst)-1)`    | 需计算剩余空间                   |
| `sprintf(buf, fmt, ...)` | 不检查 buf 大小 | `snprintf(buf, sizeof(buf), fmt, ...)`  | snprintf 保证 `\0` 结尾（POSIX） |
| `scanf("%s", buf)`       | 不检查 buf 大小 | `scanf("%15s", buf)` 或 `fgets`         | 宽度限定符只限字符数，不含 `\0`  |
| `vsprintf(buf, fmt, va)` | 不检查 buf 大小 | `vsnprintf(buf, sizeof(buf), fmt, va)`  | 同 snprintf                      |

#### 五条金科玉律

```
规则 1: 永远使用带边界检查的函数
  ❌ gets(buf)
  ✅ fgets(buf, sizeof(buf), stdin)

规则 2: 编译时启用所有安全选项
  ✅ -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE -pie

规则 3: 永远不要假设输入数据的长度是安全的
  ❌ "用户名不会超过 32 字节"
  ✅ 无论如何都使用 sizeof() 或已知边界

规则 4: 理解栈帧布局有助于写出更安全的代码
  ✅ 知道 buf 溢出后会覆盖什么 → 知道如何防护

规则 5: 代码审查时特别关注危险函数
  ✅ grep -nE '\bgets\b|\bstrcpy\b|\bstrcat\b|\bsprintf\b|\bscanf\("%s"\)' *.c
```

### 常见错误

| 错误                                              | 后果                  | 正确做法                                                      |
| ------------------------------------------------- | --------------------- | ------------------------------------------------------------- |
| 偏移量计算忘记栈对齐                              | 低估溢出所需字节数    | x86-64 ABI 要求 16 字节对齐；char buf[16] 后通常有 8 字节填充 |
| 搞反栈增长方向                                    | 误解溢出覆盖范围      | 栈向低地址增长，数组向高地址增长；溢出覆盖高地址内容          |
| canary 位置放错                                   | 错误的防护模型        | canary 在 rbp 之下、局部变量之上；不在返回地址之上            |
| 误用 `fgets(buf, 256, stdin)` 而 buf 只有 16 字节 | 人为制造溢出          | 始终使用 `sizeof(buf)` 作为 fgets 的 size 参数                |
| 认为 `strncpy` 绝对安全                           | 可能不 `\0` 结尾      | 手动置 `\0`: `strncpy(dst, src, n-1); dst[n-1]='\0';`         |
| 忘记 canary 最低字节是 `\0`                       | 攻击 payload 意外截断 | 构造 payload 时考虑 `\0` 字节对字符串函数的影响               |
| 只依赖单一防护机制                                | 单点故障导致全部失守  | 纵深防御：同时启用 Canary + ASLR + DEP + RELRO                |
| 用 `scanf("%s", buf)` 读字符串                    | 和 gets 一样危险      | 使用宽度限定符 `scanf("%15s", buf)` 或改用 fgets              |

### 重要知识点

1. **缓冲区溢出是栈布局问题的直接结果**：理解栈帧布局 = 理解为什么溢出会发生。数组从低地址向高地址增长，而栈从高地址向低地址增长，这意味着越界写入会覆盖调用者的关键数据。

2. **偏移量 = 缓冲区大小 + 对齐填充 + 保存的寄存器 + canary**：这不是固定值，取决于编译器版本、优化级别和目标架构。本题中的数值是教学示例，实际偏移应通过 objdump 确认。

3. **Canary 不是银弹**：它只能检测顺序溢出，且可被信息泄露绕过。现代攻击链（Info Leak → Canary bypass → ROP → DEP bypass）展示了纵深防御的必要性。

4. **`gets()` 是 C 语言安全史上最大的污点**：C11 正式移除它，但 GCC 为向后兼容仍支持（带警告）。任何使用 `gets()` 的代码都应立即修复。

5. **`sizeof(buf)` 是 C 程序员最强大的安全工具**：正确使用它可以自动适配缓冲区大小，避免硬编码长度带来的不一致。

6. **安全编程不是"安全函数 vs 不安全函数"的二元选择**：即使使用 `strncpy`，如果 `n` 参数错误，仍然可能溢出。真正的安全来自理解内存布局和边界条件。

7. **编译器是你的安全盟友**：现代 GCC/Clang 默认启用 `-fstack-protector-strong`，加上 `-D_FORTIFY_SOURCE=2`，可以在编译时和运行时捕捉大量漏洞。

8. **CTF 思维 vs 安全工程思维**：本题培养的是后者——不是学习如何利用漏洞，而是理解漏洞产生的根源，从而在设计和编码阶段避免它们。

### 课堂讨论

1. **为什么 C11 要移除 `gets()` 而不是修复它？**

   `gets()` 的接口设计从根本上就是有缺陷的——它只有一个参数（缓冲区指针），没有缓冲区大小信息。要"修复"它必须改变函数签名（增加 size 参数），这实际上就是 `fgets()` 的接口。标准化委员会选择移除它并引导程序员使用 `fgets()`，而不是维护一个向后兼容的危险接口。

2. **如果攻击者已经知道 canary 的值，canary 还剩下什么保护作用？**

   几乎没有。canary 的安全模型假设攻击者不知道它的值。一旦通过信息泄露获取了 canary 值，攻击者可以在溢出 payload 中原样放回，`__stack_chk_fail` 不会被触发。这就是为什么纵深防御如此重要——ASLR 和 DEP 作为第二、第三道防线继续提供保护。

3. **为什么 fork 出来的子进程 canary 相同？这合理吗？**

   技术上合理（fork 复制整个地址空间），但安全上是一个已知弱点。`fork()` 之后子进程是父进程的精确副本，包括 TLS 中的 canary 值。对于使用 `fork()` 模型的网络服务器（如 Apache prefork），攻击者可以通过反复尝试逐字节破解 canary，因为每次崩溃只杀死子进程，父进程继续存活并 fork 出新的（具有相同 canary 的）子进程。

4. **`strncpy` 和 `strcpy` 的溢出风险一样吗？**

   不完全一样。`strncpy(dst, src, n)` 最多拷贝 n 个字符，如果 n ≤ dst 的大小则不会溢出。但 `strncpy` 有一个隐藏陷阱：如果 `strlen(src) ≥ n`，则 dst 不会以 `\0` 结尾，后续的字符串操作可能读取越界。正确的安全用法是 `strncpy(dst, src, sizeof(dst)-1); dst[sizeof(dst)-1] = '\0';`。

5. **在现代 Linux 上，一个简单的栈溢出还能成功吗？**

   默认情况下非常困难。现代 Linux 发行版默认启用：Stack Canary (`-fstack-protector-strong`)、ASLR、NX-bit、PIE、Full RELRO。单独的一个 `gets()` 溢出需要同时绕过 Canary（需要信息泄露）和 ASLR（需要地址泄露），再加上 NX-bit 阻止了 shellcode 直接执行，攻击者需要构造 ROP 链。但遗留系统、IoT 设备、嵌入式系统往往没有这些防护。

6. **这道题为什么选择"分析"而不是"利用"？**

   安全教育的目的是培养安全的开发者，而非攻击者。理解溢出原理和防护机制后，开发者在写代码时就能本能地避免危险模式。相比之下，CTF 式的利用教学虽然有吸引力，但容易让学生将注意力放在攻击技巧上而非防御思维。**先学会防御，再了解攻击，这才是安全教育的正确顺序。**

### 后续衔接

- **前置课程**：Lesson 16 (my_strcpy — 理解 strcpy 的工作机制)、Lesson 26 (my_memcpy — 内存拷贝的边界概念)、Lesson 45 (realloc — 动态内存管理)
- **后续课程**：Unit 4 C Compiler (编译器如何插入 canary 检查)、Unit 5 C Kernel (内核态内存保护、MMU 和页表)
- **平行课程**：Lesson 47 (setjmp/longjmp — 非局部跳转与栈帧操作)、Lesson 48 (I/O 缓冲区性能 — 缓冲区大小与性能权衡)

### 参考文献

1. Aleph One. "Smashing The Stack For Fun And Profit." _Phrack Magazine_, Vol 7, Issue 49, 1996. — 栈溢出利用的经典开山之作
2. Cowan, C. et al. "StackGuard: Automatic Adaptive Detection and Prevention of Buffer-Overflow Attacks." _USENIX Security_, 1998. — Stack Canary 的原始论文
3. Shacham, H. "The Geometry of Innocent Flesh on the Bone: Return-into-libc without Function Calls (on the x86)." _ACM CCS_, 2007. — ROP (Return-Oriented Programming) 的奠基论文
4. CWE-120: Buffer Copy without Checking Size of Input ('Classic Buffer Overflow'). https://cwe.mitre.org/data/definitions/120.html
5. CWE-121: Stack-based Buffer Overflow. https://cwe.mitre.org/data/definitions/121.html
6. CERT C Coding Standard: STR31-C. Guarantee that storage for strings has sufficient space for character data and the null terminator.
7. MISRA C:2012, Rule 21.6 — "The Standard Library function gets shall not be used."
8. ISO/IEC 9899:2011 (C11 Standard) — Annex K (Bounds-checking interfaces) and removal of gets().
9. GCC Manual: Program Instrumentation Options — -fstack-protector, -fstack-protector-strong, -fstack-protector-all.
10. Drepper, U. "How To Write Shared Libraries." 2011. — 包含 RELRO 和 GOT 保护的详细讨论。
