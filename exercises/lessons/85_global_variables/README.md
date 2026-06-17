## Lesson 85 Global Variables 全局变量

### 代码

    /* 输入 */
    int g = 10;
    int main() { return g + 5; }

    /* 输出: 数据进 .data 段, 代码进 .text 段 */
    .data
    .globl g
    g:
        .word 10             # 4 字节初始值, 链接时分配地址

    .text
    main:
        ...
        la a0, g             # 加载 g 的"地址" (链接器填真值)
        lw a0, 0(a0)         # 再取出值 — 与局部变量殊途同归

### 知识点

- 程序的三种"住址"
  - 局部变量：栈上，`s0 - offset`, 函数返回即消失
  - 全局变量：.data 段，符号名寻址，程序整个生命周期存在
  - 代码：.text 段，只读可执行
- la 伪指令：符号 → 地址
  - 展开为 `auipc + addi` 两条指令 (PC 相对寻址)
  - 编译期不知道 g 在哪 — 汇编器留重定位项，链接器最终填址
- 初始化值放在哪
  - `int g = 10;` 的 10 直接躺在 .data 里，**不消耗任何指令**
  - 对比局部变量 `int a = 10;` 是运行期一条 store — 初始化时机完全不同
  - 未初始化全局进 .bss (loader 清零), ELF 文件里不占空间
- 符号表分层
  - parse 维护 globals 链表与函数内 locals 链表
  - 查找顺序：先局部后全局 — 局部同名变量遮蔽全局

### 课堂讨论

- `int g = f();` 全局变量用函数初始化 — C 为什么不允许？C++ 为什么允许？代价是什么？
- `la` 展开的 auipc 是"PC 相对"寻址 — 这对动态库 (PIC) 意味着什么？
- 两个 .c 文件都定义 `int g;` — 链接器会怎样？(common symbol 的历史包袱)

### 课后练习

- 测试：全局计数器被三个函数各加一次，返回 3
- 测试：局部 `int g = 99;` 遮蔽全局 g, 验证读到 99
- 用 `objdump -t` 查看 g 的符号地址; 用 `-h` 看 .data 段大小

### 参考资料

- 《程序员的自我修养》第 3 章 — 目标文件里有什么
- RISC-V ASM Manual — la/auipc 与重定位

---

### 本课文件

    test_global.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/85_global_variables/test_global.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-85.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-85.md) — 如有更新以书仓为准
