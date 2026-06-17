## Lesson 61 RISC-V Assembly Primer RISC-V 汇编入门

### 代码 1: 返回常量

    # return42.s - 最简单的 RISC-V 程序
    .globl _start
    _start:
        li a0, 42        # 将 42 放入 a0 寄存器（程序返回值）
        li a7, 93        # syscall 号 93 = exit
        ecall            # 触发系统调用

### 代码 2: 从 1 加到 100

    # sum100.s - 计算 1+2+...+100
    .globl _start
    _start:
        li t0, 0         # sum = 0
        li t1, 1         # i = 1
        li t2, 101       # 上界
    loop:
        add t0, t0, t1   # sum += i
        addi t1, t1, 1   # i++
        blt t1, t2, loop # if i < 101 goto loop

        # exit(sum & 0xFF) - 返回值只取低8位
        andi a0, t0, 0xFF
        li a7, 93
        ecall

### 代码 3: 递归阶乘

    # fact.s - 计算 fact(5) = 120
    .globl _start
    _start:
        li a0, 5         # n = 5
        call fact        # 调用 fact(5)
        li a7, 93        # exit(result)
        ecall

    fact:
        addi sp, sp, -16 # 分配栈帧
        sw ra, 12(sp)    # 保存返回地址
        sw a0, 8(sp)     # 保存参数 n

        li t0, 1
        ble a0, t0, .base # if n <= 1 goto base

        addi a0, a0, -1  # n - 1
        call fact        # fact(n-1), 结果在 a0

        lw t0, 8(sp)     # 恢复 n
        mul a0, t0, a0   # n * fact(n-1)
        j .done

    .base:
        li a0, 1         # return 1

    .done:
        lw ra, 12(sp)    # 恢复返回地址
        addi sp, sp, 16  # 释放栈帧
        ret              # 返回

### 知识点

- RISC-V RV32I 基本指令集
  - 算术：`add`, `sub`, `addi`, `mul`, `div`
  - 比较跳转：`beq`, `bne`, `blt`, `bge`, `ble`
  - 访存：`lw` (load word), `sw` (store word)
  - 伪指令：`li` (load immediate), `call`, `ret`, `mv`
- 寄存器与 ABI 命名
  - `a0-a7`: 函数参数和返回值
  - `t0-t6`: 临时寄存器 (caller-saved)
  - `s0-s11`: 保存寄存器 (callee-saved)
  - `sp`: 栈指针，`ra`: 返回地址
  - `x0`/`zero`: 硬连线为 0
- 函数调用约定
  - 调用前：参数放入 `a0-a7`, 用 `call` 跳转
  - 被调用方：保存 `ra` 和用到的 `s` 寄存器到栈
  - 返回：结果放入 `a0`, 用 `ret` 返回
- Linux 系统调用
  - `a7` = 系统调用号，`a0-a5` = 参数
  - `ecall` 触发系统调用
  - 93 = exit, 64 = write

### 编译运行

    # 汇编、链接、运行
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static return42.s -o return42
    qemu-riscv32 ./return42
    echo $?   # 输出 42

    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static sum100.s -o sum100
    qemu-riscv32 ./sum100
    echo $?   # 输出 5050 & 0xFF = 178 (返回值只有8位)

    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static fact.s -o fact
    qemu-riscv32 ./fact
    echo $?   # 输出 120

### 课堂讨论

- `x0` 寄存器硬连线为 0 有什么好处？可以用它实现哪些操作？
- 为什么 RISC-V 要区分 caller-saved 和 callee-saved 寄存器？
- `call func` 伪指令展开后是什么？和 `jal ra, func` 有什么区别？
- 如果函数参数超过 8 个怎么办？

### 课后练习

- 用 RISC-V 汇编实现斐波那契数列：`fib(10)` 返回 55
- 修改 sum100.s 使其能正确返回 5050 (提示：用 `write` 系统调用输出数字)
- 对比 Lesson 5 的 C 版本 "从 1 加到 100 求和" 和本课的汇编版本，理解 C 到汇编的映射

### 参考资料

- 《RISC-V Reader》Patterson & Waterman - 中文版 http://riscvbook.com/chinese/
- RISC-V 指令速查卡 https://github.com/jameslzhu/riscv-card
- RISC-V 在线模拟器 https://venus.cs61c.org/
