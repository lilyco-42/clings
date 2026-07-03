/* overflow_lab.c — 缓冲区溢出安全分析实验
 *
 * 任务：给定 3 段 C 代码片段 (gets 溢出/strcpy 溢出/安全版 fgets)，
 *       学生做"安全分析"——画栈帧布局 (ASCII 图)、计算偏移、
 *       判断溢出风险、解释金丝雀 (canary) 防护原理。
 *
 * 知识点：
 *   - 栈帧 (stack frame) 结构与调用约定
 *   - 缓冲区溢出 (buffer overflow) 机制
 *   - Stack canary (金丝雀) 防护原理
 *   - ASLR / DEP / NX-bit / Stack protector 防护技术对比
 *   - 安全函数 fgets vs 不安全函数 gets/strcpy
 *
 * 验证：./overflow_lab → 输出固定格式的安全分析报告
 *       判分由 clings 捕获程序 stdout 与内置用例逐行比对（clings tests 61 查看期望输出）
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* ─── 栈帧分析数据结构 ───
 *
 * 栈帧 (stack frame) 是每次函数调用时在栈上分配的一块内存区域，
 * 包含：局部变量、保存的寄存器、返回地址等。
 *
 * 典型 x86-64 栈帧布局 (从高地址到低地址):
 *
 *   ┌──────────────────────┐ 高地址 (栈底方向)
 *   │  调用者的栈帧         │
 *   ├──────────────────────┤
 *   │  返回地址 (8 bytes)   │  ← 攻击者最想覆盖的目标
 *   ├──────────────────────┤
 *   │  保存的 rbp (8 bytes) │
 *   ├──────────────────────┤ ← rbp (frame pointer)
 *   │  canary (8 bytes)     │  ← Stack protector 插入的哨兵值
 *   ├──────────────────────┤
 *   │  局部变量 / 数组      │  ← 缓冲区通常在这里
 *   ├──────────────────────┤
 *   │  ...                  │ 低地址 (栈顶方向)
 *   └──────────────────────┘
 *
 * 在本题中我们分析一个简化模型：函数只有一个 char buf[N] 局部变量。
 */
typedef struct {
    int buf_size;      /* 缓冲区大小 (字节) */
    int offset_to_ret; /* buf 起始地址到返回地址的偏移 (字节) */
    int overflow_risk; /* 0=安全，1=有风险 */
    const char *func_name;
    const char *description;
} StackAnalysis;

/* ─── 打印分隔线 ─── */
static void print_separator(void) { printf("══════════════════════════════════════════════════════\n"); }

/* ─── TODO 1: 打印代码片段 1 的栈帧分析 (gets 溢出) ─── */
#error TODO 1: Print stack frame analysis for Code Snippet 1 (gets overflow).
/*
 * Code Snippet 1:
 *   void vulnerable_gets(void) {
 *       char buf[16];
 *       gets(buf);   // 危险！gets 不检查边界
 *   }
 *
 * 栈帧布局 (x86-64, 无 canary, buf[16]):
 *
 *   地址偏移   内容              说明
 *   ───────────────────────────────────────
 *   +24      返回地址 (8B)      ← gets 可覆盖这里
 *   +16      保存的 rbp (8B)
 *   +0       buf[16]            ← gets 从这里写入 (buf[0..15])
 *
 * buf 起始到返回地址的偏移 = 16(buf) + 8(rbp) = 24 字节
 * (x86-64 -O0: buf 位于 rbp-16, 返回地址位于 rbp+8; buf[16] 已对齐, 无需填充)
 *
 * 要求：
 *   1. 打印代码片段 (ASCII 框)
 *   2. 画出 ASCII 栈帧布局图
 *   3. 计算 buf 到返回地址的偏移量
 *   4. 判断是否存在溢出风险 (是/否，并解释)
 *   5. 分析 gets 为什么危险，给出攻击示意
 *
 * 格式参考：见 README 的期望输出示例，或运行 clings tests 61 查看完整期望
 */

/* ─── TODO 2: 打印代码片段 2 的栈帧分析 (strcpy 溢出) ─── */
#error TODO 2: Print stack frame analysis for Code Snippet 2 (strcpy overflow).
/*
 * Code Snippet 2:
 *   void vulnerable_strcpy(char *src) {
 *       char buf[32];
 *       strcpy(buf, src);  // 危险！strcpy 不检查边界
 *   }
 *
 * 栈帧布局 (x86-64, 无 canary, buf[32]):
 *
 *   地址偏移   内容              说明
 *   ───────────────────────────────────────
 *   +40      返回地址 (8B)      ← strcpy 可覆盖这里
 *   +32      保存的 rbp (8B)
 *   +0       buf[32]            ← strcpy 从这里写入
 *
 * buf 起始到返回地址的偏移 = 40 字节 (buf[0..31] + rbp8)
 *
 * 要求：
 *   1. 打印代码片段
 *   2. 画出 ASCII 栈帧布局图
 *   3. 计算 buf 到返回地址的偏移量
 *   4. 判断是否存在溢出风险
 *   5. 对比 gets 和 strcpy 溢出场景的异同 (输入源、终止条件)
 *
 * 格式参考：见 README 的期望输出示例，或运行 clings tests 61 查看完整期望
 */

/* ─── TODO 3: 打印代码片段 3 的栈帧分析 (fgets 安全版) ─── */
#error TODO 3: Print stack frame analysis for Code Snippet 3 (fgets safe version).
/*
 * Code Snippet 3:
 *   void safe_fgets(void) {
 *       char buf[16];
 *       fgets(buf, sizeof(buf), stdin);  // 安全！限定了最大长度
 *   }
 *
 * 栈帧布局 (与片段 1 相同，但行为不同):
 *
 *   地址偏移   内容              说明
 *   ───────────────────────────────────────
 *   +24      返回地址 (8B)      ← fgets 无法覆盖这里
 *   +16      保存的 rbp (8B)
 *   +8       [填充/对齐] (8B)
 *   +0       buf[16]            ← fgets 最多读入 15 字节
 *
 * fgets(buf, 16, stdin) 最多读入 15 个字符 + '\0'。
 * 因此永远不会写入 buf[16] 之外，返回地址安全。
 *
 * 要求：
 *   1. 打印代码片段
 *   2. 画出 ASCII 栈帧布局图
 *   3. 计算 buf 到返回地址的偏移量
 *   4. 判断是否存在溢出风险
 *   5. 解释 fgets 如何防止溢出 (size 参数机制)
 *
 * 格式参考：见 README 的期望输出示例，或运行 clings tests 61 查看完整期望
 */

/* ─── TODO 4: 金丝雀 (Stack Canary) 防护原理 ─── */
#error TODO 4: Explain stack canary protection mechanism.
/*
 * Stack Canary (栈金丝雀 / 栈哨兵):
 *
 * 编译器在栈帧中返回地址前插入一个随机值 (canary)。
 * 函数返回前检查 canary 是否被修改：
 *   - 如果 canary 完好 → 正常返回
 *   - 如果 canary 被覆盖 → 程序终止 (__stack_chk_fail)
 *
 * 金丝雀名称来源于煤矿工人带金丝雀下矿——
 * 金丝雀对毒气敏感，先死掉就警告矿工撤离。
 *
 * 栈帧布局 (带 canary):
 *
 *   地址偏移   内容
 *   ───────────────────
 *   +32      返回地址 (8B)
 *   +24      保存的 rbp (8B)
 *   +16      canary (8B)     ← 哨兵值 (通常以 \0 开头防字符串泄露)
 *   +8       [填充/对齐] (8B)
 *   +0       buf[16]
 *
 * canary 特征：
 *   - 随机值，每次程序运行都不同
 *   - 最低字节为 \0 (防止通过字符串函数泄露)
 *   - 从 fs:0x28 (x86-64) 或 gs:0x14 (x86) 读取
 *   - GCC 用 -fstack-protector 启用
 *
 * 要求：
 *   1. 解释 canary 的名称由来 (煤矿金丝雀故事)
 *   2. 画出带 canary 的栈帧布局
 *   3. 说明 canary 检测流程 (入口/返回前)
 *   4. 说明 canary 的局限性 (信息泄露、暴力猜测等)
 *   5. 列出绕过 canary 的方法 (至少 3 种)
 *
 * 格式参考：见 README 的期望输出示例，或运行 clings tests 61 查看完整期望
 */

/* ─── TODO 5: 防护机制对比表 ─── */
#error TODO 5: Print a comparison table of protection mechanisms.
/*
 * 六种主要防护机制：
 *   1. Stack Canary (栈金丝雀)
 *   2. ASLR (Address Space Layout Randomization, 地址空间布局随机化)
 *   3. DEP/NX-bit (Data Execution Prevention / No-eXecute)
 *   4. RELRO (Relocation Read-Only)
 *   5. PIE (Position Independent Executable)
 *   6. FORTIFY_SOURCE (编译时库函数强化)
 *
 * 要求：
 *   1. 打印表格：防护机制 | 防护目标 | 原理 | 局限性
 *   2. 覆盖 Canary, ASLR, DEP/NX, RELRO, PIE, FORTIFY_SOURCE
 *   3. 说明纵深防御 (defense in depth) 的概念
 *   4. 画出纵深防御示意 (多层防护链)
 *
 * 格式参考：见 README 的期望输出示例，或运行 clings tests 61 查看完整期望
 */

/* ─── TODO 6: 综合风险评估 ─── */
#error TODO 6: Print overall risk assessment and summary.
/*
 * 对三段代码片段给出综合风险评估：
 *   - 片段 1 (gets):  严重 (CRITICAL) — 无边界检查，经典溢出入口
 *   - 片段 2 (strcpy): 高危 (HIGH) — 依赖外部输入长度
 *   - 片段 3 (fgets):  安全 (SAFE) — 边界受控
 *
 * 总结安全编程准则：
 *   1. 永远使用带边界检查的函数 (fgets, strncpy, snprintf)
 *   2. 编译时启用栈保护 (-fstack-protector-strong)
 *   3. 不要假设输入数据长度
 *   4. 理解栈帧布局有助于写出更安全的代码
 *   5. 代码审查时特别关注危险函数
 *
 * 格式参考：见 README 的期望输出示例，或运行 clings tests 61 查看完整期望
 */

/* ─── TODO 7: 打印学习建议 ─── */
#error TODO 7: Print further learning suggestions.
/*
 * 推荐进一步学习：
 *   1. 在 Linux 上用 -fno-stack-protector 编译并观察溢出效果
 *   2. 使用 objdump -d 查看栈帧布局
 *   3. 阅读 CWE-120: Buffer Copy without Checking Size of Input
 *   4. 了解现代编译器默认启用的安全选项
 *   5. 推荐经典论文阅读
 *
 * 格式参考：见 README 的期望输出示例，或运行 clings tests 61 查看完整期望
 */

int main(void) {
#error TODO 1-7: Complete all analysis sections above.
    return 0;
}
