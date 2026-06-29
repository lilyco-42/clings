## Lesson 82 Function Definitions 函数定义

### 代码

    /* 输入 */
    int double_it() { return 21 * 2; }
    int main() { return double_it(); }

    /* 每个函数 = 独立标号 + 独立栈帧 */
    .globl double_it
    double_it:
        addi sp, sp, -16     # prologue: 自己的帧
        sw ra, 0(sp)
        sw s0, 4(sp)
        addi s0, sp, 16
        li a0, 42
        j .L_return_double_it
    .L_return_double_it:     # epilogue: 唯一出口
        lw ra, 0(sp)
        lw s0, 4(sp)
        addi sp, sp, 16
        ret

    main:
        ...
        call double_it       # ra = 下一条指令地址, 跳转
        ...                  # 返回值已在 a0

### 知识点

- call/ret 与 ra 寄存器
  - `call f` 是伪指令：`auipc ra, ...; jalr ra, ...` — 把返回地址写进 ra
  - `ret` 即 `jalr x0, 0(ra)` — 跳回 ra
  - **嵌套调用会覆盖 ra** → 这就是 prologue 必须保存 ra 的原因
- 函数自治：每个函数一套 prologue/epilogue
  - 调用者不关心被调函数用多大栈帧 — sp 进出平衡即可
  - 这是"调用约定"的雏形：Lesson 83 引入参数后完整成形
- 统一出口 `.L_return_<fn>`
  - 函数体内任何 `return` 都跳到这里，epilogue 只写一份
  - 多出口函数 (循环里 return) 不会漏恢复寄存器
- 符号表按函数隔离
  - 每个函数独立的局部变量链表与 stack_size
  - 函数名进全局符号表 — `.globl` 导出给链接器

### 课堂讨论

- 如果 double_it 里再调用别的函数，但 prologue 不保存 ra — 会发生什么？画出调用链
- 为什么返回值约定用 a0 而不是栈？多返回值语言 (Go) 怎么做？
- `call` 展开成 `auipc+jalr` 两条指令 — 为什么一条 `jal` 不够？(±1MB 跳转范围)

### 课后练习

- 测试：三个函数 a()→b()→c() 链式调用，验证 ra 保存的必要性
- 测试：函数定义在调用点之后 (无前向声明) — 本编译器支持吗？为什么？
- 用 `riscv64-linux-gnu-objdump -d` 看 `call` 真实展开成了什么

### 参考资料

- RISC-V Unprivileged ISA — Control Transfer Instructions
- chibicc step "Add zero-arity function calls"

---

### 本课文件

    test_func.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/82_function_definitions/test_func.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-82.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-82.md) — 如有更新以书仓为准
