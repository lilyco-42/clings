/* overflow_lab.c — 缓冲区溢出安全分析实验 (solution) */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* ─── 栈帧分析数据结构 ─── */
typedef struct {
    int buf_size;
    int offset_to_ret;
    int overflow_risk;
    const char *func_name;
    const char *description;
} StackAnalysis;

/* ─── 打印分隔线 ─── */
static void print_separator(void) { printf("══════════════════════════════════════════════════════\n"); }

int main(void) {
    /* ══════════════════════════════════════════════════════
     *  缓冲区溢出安全分析实验报告
     * ══════════════════════════════════════════════════════ */
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║      缓冲区溢出安全分析实验报告                      ║\n");
    printf("║      Buffer Overflow Security Analysis Lab           ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  预备知识：栈帧 (Stack Frame) 结构
     * ══════════════════════════════════════════════════════ */
    printf("【预备知识】x86-64 栈帧结构\n");
    printf("\n");
    printf("  每当函数被调用时，CPU 在栈上分配一块内存区域，称为\"栈帧\"。\n");
    printf("  栈帧包含：局部变量、保存的寄存器、返回地址等。\n");
    printf("  栈从高地址向低地址增长 (x86-64)。\n");
    printf("\n");
    printf("  典型栈帧布局 (从高地址到低地址):\n");
    printf("\n");
    printf("      高地址 (栈底方向)\n");
    printf("      ┌──────────────────────┐\n");
    printf("      │  调用者的栈帧         │\n");
    printf("      ├──────────────────────┤\n");
    printf("      │  返回地址 (8 bytes)   │ ← RIP: 被调用函数返回后执行哪里\n");
    printf("      ├──────────────────────┤\n");
    printf("      │  保存的 rbp (8 bytes) │ ← 调用者的栈帧基址\n");
    printf("      ├──────────────────────┤ ← rbp (frame pointer)\n");
    printf("      │  canary (8 bytes)     │ ← Stack protector 哨兵 (可选)\n");
    printf("      ├──────────────────────┤\n");
    printf("      │  局部变量 / 数组      │ ← 缓冲区通常在这里\n");
    printf("      ├──────────────────────┤\n");
    printf("      │  callee-saved 寄存器  │ (可选)\n");
    printf("      ├──────────────────────┤\n");
    printf("      │  函数参数 (第 7 个起)   │ (寄存器传参的前 6 个不在栈上)\n");
    printf("      └──────────────────────┘\n");
    printf("      低地址 (栈顶方向) ← rsp\n");
    printf("\n");
    printf("  缓冲区溢出本质：向低地址方向的缓冲区写入超过其容量的数据，\n");
    printf("                  逐字节覆盖高地址方向的敏感数据 (canary/rbp/ret)。\n");
    printf("\n");
    print_separator();
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  代码片段 1: gets() 缓冲区溢出
     * ══════════════════════════════════════════════════════ */
    printf("【代码片段 1】gets() 缓冲区溢出 — 风险等级：CRITICAL\n");
    printf("\n");
    printf("  源代码:\n");
    printf("  ┌──────────────────────────────────────────────────┐\n");
    printf("  │  void vulnerable_gets(void) {                    │\n");
    printf("  │      char buf[16];                               │\n");
    printf("  │      gets(buf);  // 危险！无边界检查             │\n");
    printf("  │  }                                               │\n");
    printf("  └──────────────────────────────────────────────────┘\n");
    printf("\n");

    printf("  栈帧布局分析 (x86-64, 无 canary):\n");
    printf("\n");
    printf("      地址偏移       内容                说明\n");
    printf("      ─────────────────────────────────────────────────\n");
    printf("      +24           ┌──────────────┐\n");
    printf("                    │  返回地址     │ ← gets 溢出可覆盖!\n");
    printf("      +16           ├──────────────┤\n");
    printf("                    │  保存的 rbp   │\n");
    printf("      +8            ├──────────────┤\n");
    printf("                    │  [栈对齐填充]  │  (8 bytes, 对齐到 16 字节)\n");
    printf("      +0            ├──────────────┤\n");
    printf("                    │  buf[0..15]   │ ← gets() 从这里开始写入\n");
    printf("                    └──────────────┘\n");
    printf("\n");
    printf("      偏移计算:\n");
    printf("        buf[0] 到返回地址的距离 = 16 (buf) + 8 (对齐) + 8 (rbp) = 32 字节\n");
    printf("        (注：若编译器将 buf 紧挨 rbp 放置则为 24 字节，\n");
    printf("         此处展示典型对齐后的布局)\n");
    printf("\n");
    printf("      风险判断：✗ 存在溢出风险\n");
    printf("        gets() 从 stdin 读取直到换行符，完全不检查 buf 的大小。\n");
    printf("        输入超过 15 个字符即可覆盖返回地址，实现控制流劫持。\n");
    printf("        这是 C 标准库中最危险的函数之一 (C11 已移除 gets)。\n");
    printf("\n");
    printf("      攻击示意:\n");
    printf("        输入：'A' × 32 + 恶意地址 (8 bytes)\n");
    printf("              └── 填充 buf+rbp ──┘└─ 覆盖返回地址 ─┘\n");
    printf("        结果：函数\"返回\"到攻击者指定的地址 → 代码执行劫持\n");
    printf("\n");
    print_separator();
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  代码片段 2: strcpy() 缓冲区溢出
     * ══════════════════════════════════════════════════════ */
    printf("【代码片段 2】strcpy() 缓冲区溢出 — 风险等级：HIGH\n");
    printf("\n");
    printf("  源代码:\n");
    printf("  ┌──────────────────────────────────────────────────┐\n");
    printf("  │  void vulnerable_strcpy(char *src) {             │\n");
    printf("  │      char buf[32];                               │\n");
    printf("  │      strcpy(buf, src);  // 危险！不检查边界      │\n");
    printf("  │  }                                               │\n");
    printf("  └──────────────────────────────────────────────────┘\n");
    printf("\n");

    printf("  栈帧布局分析 (x86-64, 无 canary):\n");
    printf("\n");
    printf("      地址偏移       内容                说明\n");
    printf("      ─────────────────────────────────────────────────\n");
    printf("      +40           ┌──────────────┐\n");
    printf("                    │  返回地址     │ ← strcpy 溢出可覆盖!\n");
    printf("      +32           ├──────────────┤\n");
    printf("                    │  保存的 rbp   │\n");
    printf("      +0            ├──────────────┤\n");
    printf("                    │  buf[0..31]   │ ← strcpy() 从这里开始写入\n");
    printf("                    └──────────────┘\n");
    printf("\n");
    printf("      偏移计算:\n");
    printf("        buf[0] 到返回地址的距离 = 32 (buf) + 8 (rbp) = 40 字节\n");
    printf("        (32 字节已对齐到 16 字节，不需要额外填充)\n");
    printf("\n");
    printf("      风险判断：✗ 存在溢出风险\n");
    printf("        strcpy() 拷贝直到遇到 '\\0'，不检查目标缓冲区大小。\n");
    printf("        若 src 长度 ≥ 32，则会覆盖 rbp 和返回地址。\n");
    printf("        风险略低于 gets(): 攻击者需要控制 src 的内容和长度，\n");
    printf("        且 '\\0' 字节可能限制某些攻击载荷。\n");
    printf("\n");
    printf("      gets vs strcpy 溢出对比:\n");
    printf("        ┌──────────┬──────────────┬──────────────┐\n");
    printf("        │ 函数     │ 输入源        │ 终止条件     │\n");
    printf("        ├──────────┼──────────────┼──────────────┤\n");
    printf("        │ gets()   │ stdin (交互)  │ 换行符 '\\n' │\n");
    printf("        │ strcpy() │ 字符串参数    │ 空字符 '\\0' │\n");
    printf("        └──────────┴──────────────┴──────────────┘\n");
    printf("\n");
    print_separator();
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  代码片段 3: fgets() 安全版本
     * ══════════════════════════════════════════════════════ */
    printf("【代码片段 3】fgets() 安全版本 — 风险等级：SAFE\n");
    printf("\n");
    printf("  源代码:\n");
    printf("  ┌──────────────────────────────────────────────────┐\n");
    printf("  │  void safe_fgets(void) {                         │\n");
    printf("  │      char buf[16];                               │\n");
    printf("  │      fgets(buf, sizeof(buf), stdin);  // 安全！  │\n");
    printf("  │  }                                               │\n");
    printf("  └──────────────────────────────────────────────────┘\n");
    printf("\n");

    printf("  栈帧布局分析 (x86-64, 无 canary):\n");
    printf("\n");
    printf("      地址偏移       内容                说明\n");
    printf("      ─────────────────────────────────────────────────\n");
    printf("      +24           ┌──────────────┐\n");
    printf("                    │  返回地址     │ ← fgets 无法到达!\n");
    printf("      +16           ├──────────────┤\n");
    printf("                    │  保存的 rbp   │ ← fgets 无法到达!\n");
    printf("      +8            ├──────────────┤\n");
    printf("                    │  [栈对齐填充]  │ ← fgets 无法到达!\n");
    printf("      +0            ├──────────────┤\n");
    printf("                    │  buf[0..15]   │ ← fgets 最多写 15+1 字节\n");
    printf("                    └──────────────┘\n");
    printf("\n");
    printf("      偏移计算:\n");
    printf("        buf[0] 到返回地址的距离 = 16 + 8 + 8 = 32 字节\n");
    printf("        fgets(buf, 16, stdin) 最多读入 15 个字符 + '\\0'\n");
    printf("        → 最多写入 buf[0..15]，无法触及返回地址区域\n");
    printf("\n");
    printf("      风险判断：✓ 安全 (在正确使用 sizeof 的前提下)\n");
    printf("        fgets 接受最大长度参数，保证不会越界写入。\n");
    printf("        注意：若程序员错误地传入大于实际缓冲区的 size，\n");
    printf("              仍可能溢出。正确做法：fgets(buf, sizeof(buf), stdin)\n");
    printf("\n");
    printf("      fgets 安全机制:\n");
    printf("        1. 第 2 个参数指定最多读取 (size-1) 个字符\n");
    printf("        2. 总是以 '\\0' 结尾 (若 size > 0)\n");
    printf("        3. 遇到换行符或 EOF 也会停止\n");
    printf("        4. 不会写入超过 size 字节的数据\n");
    printf("\n");
    print_separator();
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  金丝雀 (Stack Canary) 防护原理
     * ══════════════════════════════════════════════════════ */
    printf("【深入分析】金丝雀 (Stack Canary) 防护原理\n");
    printf("\n");
    printf("  名称由来:\n");
    printf("    煤矿工人下矿时携带金丝雀。金丝雀对瓦斯 (一氧化碳) 极为敏感，\n");
    printf("    一旦金丝雀死亡，矿工就知道有毒气泄漏，立即撤离。\n");
    printf("    类似地，stack canary 是栈上的\"哨兵\"——一旦被覆盖，\n");
    printf("    程序就知道发生了缓冲区溢出，立即终止。\n");
    printf("\n");
    printf("  带 Canary 的栈帧布局:\n");
    printf("\n");
    printf("      地址偏移       内容                说明\n");
    printf("      ─────────────────────────────────────────────\n");
    printf("      +32           ┌──────────────┐\n");
    printf("                    │  返回地址     │ ← 攻击者的最终目标\n");
    printf("      +24           ├──────────────┤\n");
    printf("                    │  保存的 rbp   │\n");
    printf("      +16           ├──────────────┤\n");
    printf("                    │  CANARY       │ ← ★ 哨兵值 (8 bytes)\n");
    printf("      +8            ├──────────────┤\n");
    printf("                    │  [栈对齐填充]  │\n");
    printf("      +0            ├──────────────┤\n");
    printf("                    │  buf[0..15]   │ ← 溢出从这里开始\n");
    printf("                    └──────────────┘\n");
    printf("\n");
    printf("  Canary 工作流程:\n");
    printf("\n");
    printf("    函数入口:\n");
    printf("      1. 从 TLS (Thread-Local Storage) 读取随机 canary 值\n");
    printf("         x86-64: mov rax, QWORD PTR fs:0x28\n");
    printf("      2. 将 canary 写入栈帧 (rbp-8 位置)\n");
    printf("\n");
    printf("    函数返回前:\n");
    printf("      3. 从栈帧读取 canary 值\n");
    printf("      4. 与 TLS 中的原始值比较 (xor 指令)\n");
    printf("      5. 若相同 → 正常返回 (ret)\n");
    printf("      6. 若不同 → 调用 __stack_chk_fail() → 程序终止\n");
    printf("\n");
    printf("  Canary 特征:\n");
    printf("    • 随机值 — 每次程序运行都不同 (防止固定值猜测)\n");
    printf("    • 最低字节为 \\0 (NULL) — 阻止通过字符串函数泄露\n");
    printf("      因为 strcpy/gets 遇到 \\0 会停止拷贝\n");
    printf("    • 存储在 TLS (fs:0x28 on x86-64, gs:0x14 on x86)\n");
    printf("    • GCC 编译选项：-fstack-protector / -fstack-protector-strong\n");
    printf("\n");
    printf("  Canary 的局限性:\n");
    printf("    1. 信息泄露：若攻击者能先读取 canary 值，可构造精确覆盖\n");
    printf("       (Format string 漏洞可泄露栈内容)\n");
    printf("    2. 非连续覆盖：canary 只能检测顺序溢出，无法检测\n");
    printf("       直接写入返回地址的漏洞 (如任意地址写)\n");
    printf("    3. 暴力破解：fork 出的子进程 canary 相同，\n");
    printf("       攻击者可逐字节暴力猜测 (耗时但可行)\n");
    printf("    4. 不覆盖返回地址：覆盖函数指针、数据指针等其他目标\n");
    printf("       可能绕过 canary 保护\n");
    printf("\n");
    printf("  绕过 Canary 的方法:\n");
    printf("    a) 信息泄露 + 精确覆盖：先泄露 canary 值，写入时原样放回\n");
    printf("    b) 改写 GOT 表：不碰 canary，直接覆盖全局偏移表\n");
    printf("    c) 覆盖局部变量：修改函数指针或关键判断变量\n");
    printf("    d) 逐字节暴力破解：fork 服务器场景下 canary 不变\n");
    printf("\n");
    print_separator();
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  防护机制对比表
     * ══════════════════════════════════════════════════════ */
    printf("【防护机制对比】纵深防御体系\n");
    printf("\n");
    printf("  现代操作系统采用多层防护 (Defense in Depth)，\n");
    printf("  每层防护针对不同攻击阶段:\n");
    printf("\n");
    printf("  ┌──────────────┬──────────────┬────────────────────────┬──────────────────┐\n");
    printf("  │ 防护机制      │ 防护目标      │ 原理                   │ 局限性            │\n");
    printf("  ├──────────────┼──────────────┼────────────────────────┼──────────────────┤\n");
    printf("  │ Stack Canary │ 栈缓冲区溢出  │ 返回地址前放随机哨兵值   │ 信息泄露可绕过    │\n");
    printf("  │              │ 检测          │ 返回前检查是否被修改     │ 不保护数据指针    │\n");
    printf("  ├──────────────┼──────────────┼────────────────────────┼──────────────────┤\n");
    printf("  │ ASLR         │ 代码复用攻击  │ 随机化栈/堆/库/代码段    │ 信息泄露可绕过    │\n");
    printf("  │              │ (ROP/JOP)    │ 基地址，使地址不可预测    │ 熵不足 (32-bit)  │\n");
    printf("  ├──────────────┼──────────────┼────────────────────────┼──────────────────┤\n");
    printf("  │ DEP / NX-bit │ 代码注入攻击  │ 标记数据页不可执行       │ ROP 可绕过       │\n");
    printf("  │              │ (shellcode)  │ 栈/堆不能同时写 + 执行     │ JIT 需要可执行堆  │\n");
    printf("  ├──────────────┼──────────────┼────────────────────────┼──────────────────┤\n");
    printf("  │ RELRO        │ GOT 表覆写   │ 重定位表只读            │ 不保护栈溢出     │\n");
    printf("  │              │ 攻击          │ 延迟绑定的 PLT 仍可写    │ Partial RELRO    │\n");
    printf("  ├──────────────┼──────────────┼────────────────────────┼──────────────────┤\n");
    printf("  │ PIE          │ 代码段固定    │ 可执行文件编译为         │ 轻微性能开销     │\n");
    printf("  │              │ 地址攻击      │ 位置无关代码             │ 需 ASLR 配合     │\n");
    printf("  ├──────────────┼──────────────┼────────────────────────┼──────────────────┤\n");
    printf("  │ FORTIFY      │ 不安全库函数  │ 编译时替换为带边界        │ 需源码重编译     │\n");
    printf("  │ _SOURCE      │ 调用          │ 检查的版本 (__chk)      │ 不能检测所有情况  │\n");
    printf("  └──────────────┴──────────────┴────────────────────────┴──────────────────┘\n");
    printf("\n");
    printf("  纵深防御示意:\n");
    printf("\n");
    printf("    攻击者 ──→ [Canary] ──→ [ASLR] ──→ [DEP/NX] ──→ [RELRO] ──→ 目标\n");
    printf("               第 1 层       第 2 层      第 3 层        第 4 层\n");
    printf("              (检测溢出)  (隐藏地址)  (阻止执行)   (保护 GOT)\n");
    printf("\n");
    printf("  绕过所有防护需要：信息泄露 + ROP 链 + GOT 覆写 → 攻击复杂度极高\n");
    printf("\n");
    print_separator();
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  综合风险评估
     * ══════════════════════════════════════════════════════ */
    printf("【综合风险评估】\n");
    printf("\n");
    printf("  ┌─────────────────┬──────────┬──────────────────────────────┐\n");
    printf("  │ 代码片段         │ 风险等级  │ 原因                         │\n");
    printf("  ├─────────────────┼──────────┼──────────────────────────────┤\n");
    printf("  │ 片段 1: gets()   │ CRITICAL │ 无任何边界检查               │\n");
    printf("  │                 │          │ 攻击者通过 stdin 直接控制     │\n");
    printf("  │                 │          │ C11 标准已移除该函数          │\n");
    printf("  ├─────────────────┼──────────┼──────────────────────────────┤\n");
    printf("  │ 片段 2: strcpy() │ HIGH     │ 无边界检查                   │\n");
    printf("  │                 │          │ 需控制 src 内容 (含 \\0 限制) │\n");
    printf("  │                 │          │ 广泛存在于遗留代码中          │\n");
    printf("  ├─────────────────┼──────────┼──────────────────────────────┤\n");
    printf("  │ 片段 3: fgets()  │ SAFE     │ 显式指定最大长度              │\n");
    printf("  │                 │          │ 正确使用 sizeof 即可保证安全  │\n");
    printf("  │                 │          │ 推荐作为标准输入的首选函数    │\n");
    printf("  └─────────────────┴──────────┴──────────────────────────────┘\n");
    printf("\n");

    printf("  【安全编程准则】\n");
    printf("\n");
    printf("  1. 永远使用带边界检查的函数:\n");
    printf("     替代 gets()   → fgets(buf, sizeof(buf), stdin)\n");
    printf("     替代 strcpy() → strncpy(buf, src, sizeof(buf)-1); buf[sizeof(buf)-1]='\\0';\n");
    printf("     替代 sprintf()→ snprintf(buf, sizeof(buf), fmt, ...)\n");
    printf("     替代 strcat() → strncat(buf, src, sizeof(buf)-strlen(buf)-1);\n");
    printf("\n");
    printf("  2. 编译时启用安全选项:\n");
    printf("     gcc -fstack-protector-strong  (启用栈保护)\n");
    printf("     gcc -D_FORTIFY_SOURCE=2       (启用 fortified 函数)\n");
    printf("     gcc -fPIE -pie                (位置无关可执行文件)\n");
    printf("     gcc -Wformat -Wformat-security (格式字符串检查)\n");
    printf("\n");
    printf("  3. 永远不要假设输入数据的长度是安全的\n");
    printf("  4. 理解栈帧布局有助于写出更安全的代码\n");
    printf("  5. 代码审查时特别关注：gets, strcpy, strcat, sprintf, scanf(\"%%s\")\n");
    printf("\n");
    print_separator();
    printf("\n");

    /* ══════════════════════════════════════════════════════
     *  进一步学习建议
     * ══════════════════════════════════════════════════════ */
    printf("【进一步学习建议】\n");
    printf("\n");
    printf("  1. 动手实验:\n");
    printf("     在 Linux 上用 -fno-stack-protector 编译并运行溢出代码\n");
    printf("     $ gcc -fno-stack-protector -z execstack -o vuln vuln.c\n");
    printf("     观察不同输入长度对程序行为的影响 (崩溃 vs 劫持)\n");
    printf("\n");
    printf("  2. 工具使用:\n");
    printf("     objdump -d overflow_lab  → 查看反汇编和栈帧布局\n");
    printf("     checksec --file=overflow_lab  → 查看启用的安全特性\n");
    printf("     gdb ./overflow_lab → 设置断点，观察运行时栈布局\n");
    printf("\n");
    printf("  3. 标准与规范:\n");
    printf("     CWE-120: Buffer Copy without Checking Size of Input\n");
    printf("     CWE-121: Stack-based Buffer Overflow\n");
    printf("     CERT C: STR31-C (Guarantee storage for strings)\n");
    printf("     MISRA C: Rule 21.6 (禁止使用 gets)\n");
    printf("\n");
    printf("  4. 现代编译器默认安全选项:\n");
    printf("     GCC ≥6: -fstack-protector-strong 默认开启\n");
    printf("     Clang ≥5: -fstack-protector-strong 默认开启\n");
    printf("     glibc ≥2.16: gets 声明标记为 deprecated\n");
    printf("     C11: gets 从标准中移除\n");
    printf("\n");
    printf("  5. 相关阅读:\n");
    printf("     Smashing The Stack For Fun And Profit (Aleph One, 1996)\n");
    printf("     StackGuard: Automatic Adaptive Detection of Buffer Overflow (1998)\n");
    printf("     The Geometry of Innocent Flesh on the Bone (Shacham, 2007, ROP)\n");
    printf("\n");
    print_separator();
    printf("\n");

    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  分析报告结束                                        ║\n");
    printf("║  记住：缓冲区溢出是 C 语言中最古老也最危险的安全漏洞  ║\n");
    printf("║  理解栈帧 = 理解如何防御                             ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");

    return 0;
}
