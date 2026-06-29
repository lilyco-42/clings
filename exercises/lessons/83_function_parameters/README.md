## Lesson 83 Function Parameters 函数参数

### 代码

    /* 输入 */
    int add(int a, int b) { return a + b; }
    int main() { return add(3, 4); }

    /* 调用方: 实参求值 → 装入 a0-a7 */
        li a0, 4             # 后求的实参先 push (栈式)
        addi sp, sp, -4
        sw a0, 0(sp)
        li a0, 3
        addi sp, sp, -4
        sw a0, 0(sp)
        lw a0, 0(sp)         # pop 回 a0, a1
        addi sp, sp, 4
        lw a1, 0(sp)
        addi sp, sp, 4
        call add

    /* 被调方 prologue: 寄存器参数立即落栈 */
    add:
        ...                  # 标准 prologue
        sw a0, -4(s0)        # 参数 a 落到自己帧里
        sw a1, -8(s0)        # 参数 b
        ...                  # 函数体把参数当普通局部变量

### 知识点

- RISC-V 调用约定：前 8 个整型参数走 a0-a7
  - 第 9 个起走栈 — 本编译器直接报错 (教学取舍，明确边界)
  - 返回值复用 a0 — 参数寄存器与返回寄存器是同一组
- 参数落栈：最简单的正确做法
  - a0-a7 是 caller-saved: 函数体内一旦再调用别人，参数寄存器必被覆盖
  - prologue 统一落栈后，参数 == 局部变量，后续代码零特殊化
  - 代价：每参数多一存一取 — 寄存器分配优化的起点 (思考题)
- 实参求值顺序
  - 本编译器从右到左压栈再统一弹回 — 顺序是实现自由
  - C 标准**不规定**实参求值顺序：`f(i++, i++)` 是经典 UB
- 形参与实参的偏移对接
  - 形参在 parse 期按声明序拿到 offset, 与局部变量同一套符号表

### 课堂讨论

- `add(g(), h())` — g 和 h 谁先执行？换个编译器还一样吗？写程序验证
- 为什么 a0-a7 落栈后再用，而不是直接在寄存器里用一辈子？什么时候必须落？
- x86-64 只有 6 个参数寄存器，ARM32 只有 4 个 — 第 7 个参数都去哪了？

### 课后练习

- 测试：`int max(int a, int b) { if (a > b) return a; return b; }`
- 测试：8 个参数全用上; 第 9 个参数验证报错信息
- 挑战：让 `add(1, add(2, 3))` 正确 — 嵌套调用为什么栈式传参不会乱？

### 参考资料

- RISC-V psABI — Integer Calling Convention
- N1256 6.5.2.2p10 — 实参求值顺序未指定

---

### 本课文件

    test_func.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/83_function_parameters/test_func.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-83.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-83.md) — 如有更新以书仓为准
