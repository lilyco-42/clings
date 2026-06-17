## Lesson 93 Standard Library Stubs 标准库桩函数

### 代码

    /* 输入 */
    int main() { printf("count = %d\n", 42); return 0; }

    /* 谁实现 printf? 我们自己, 用汇编 + Linux 系统调用 */

    # runtime_rv.s — 平台层: 怎么"出生"和"打印一个字符"
    .globl _start
    _start:
        call main
        li a7, 93            # syscall: exit
        ecall                # 内核, 拜托了
    .globl putchar
    putchar:
        addi sp, sp, -16
        sw a0, 0(sp)         # 字符放栈上
        li a0, 1             # fd   = stdout
        mv a1, sp            # buf  = &字符
        li a2, 1             # len  = 1
        li a7, 64            # syscall: write
        ecall
        addi sp, sp, 16
        ret

    # runtime_rv_io.s — 逻辑层: printf 只依赖 putchar
    #   print_int: 递归打印十进制 (负号 + 逐位)
    #   printf:    扫描格式串, %d→print_int, %c→putchar, %s→循环 putchar

### 知识点

- -nostdlib 的世界：没有 main 之前与 return 之后
  - 真实程序入口是 `_start` (链接器默认入口符号), 不是 main
  - main 返回后必须显式 exit 系统调用 — 否则 CPU 一头撞进非法内存
  - gcc 平时偷偷帮你链了 crt0.o 做这些事 — 本课自己当 crt0
- 系统调用：用户态与内核的边界
  - RISC-V Linux 约定：a7 放调用号 (93=exit, 64=write), a0-a2 放参数，ecall 陷入
  - printf 的尽头是 write(1, buf, len) — 所有 I/O 库的最底层
- 运行时分层 (本仓库的真实结构)
  - `runtime_rv_io.s` 纯逻辑：print_int/printf, 只调 putchar — **平台无关**
  - `runtime_rv.s` 平台层：\_start/putchar 用 Linux syscall 实现
  - 同一份 io 层配上裸机平台层 (写 UART 寄存器), 程序就能在无 OS 的板子上跑 — Lesson 96 见
- 简化 printf 的边界
  - 支持 %d %c %s 与字面字符; 不做宽度/精度/%f — 留作真 libc 的敬畏

### 课堂讨论

- `printf("%d", x)` 的 42 是怎么从 a1 走到屏幕的？画完整调用链直到 ecall
- putchar 逐字符 write 系统调用太奢侈 — 真实 libc 的缓冲区方案是什么？`\n` 为什么会触发刷新？
- exit 的退出码只取低 8 位 — 在内核哪个环节被截断的？(wait status 编码)

### 课后练习

- 测试：`printf("%s says %d\n", "nccl", 42);`
- 扩展：给 print_int 加十六进制版本 print_hex (%x)
- 用 strace 观察 qemu-riscv32 跑该程序的真实 write/exit 系统调用序列

### 参考资料

- RISC-V Linux syscall 表 (man 2 syscall / unistd.h)
- 《程序员的自我修养》第 11 章 — 运行库

---

### 在本仓验证

    cd ../../nccl-cc && make
    # 本课没有退出码型测试 — 编译一个用 printf 的程序观察输出
    printf 'int main() { printf("count = %d\n", 42); return 0; }' > /tmp/p.c
    ./nccl-cc /tmp/p.c > /tmp/p.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/p.s runtime_rv_io.s runtime_rv.s -o /tmp/p
    qemu-riscv32 /tmp/p        # => count = 42

### 本课文件

    ../../nccl-cc/runtime_rv.s     — Linux 平台层: _start / putchar (syscall)
    ../../nccl-cc/runtime_rv_io.s  — 平台无关层: print_int / printf

> 课文源：[NCCL/Unit-4/Lesson-93.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-93.md) — 如有更新以书仓为准
