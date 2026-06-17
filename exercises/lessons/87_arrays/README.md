## Lesson 87 Arrays 数组

### 代码

    /* 输入 */
    int main() { int a[10]; a[3] = 42; return a[3]; }

    /* a[3] 的地址计算: 基址 + 3×4 */
        addi a0, s0, -40     # a 的基址 (数组占 s0-40 .. s0-4)
        addi sp, sp, -4      # push 基址
        sw a0, 0(sp)
        li a0, 3
        li t0, 4
        mul a0, a0, t0       # 下标 × 元素大小 (parse 期插入的乘法)
        lw t0, 0(sp)
        addi sp, sp, 4
        add a0, a0, t0       # 基址 + 偏移 = &a[3]
        ...                  # 作为左值: sw / 作为右值: lw

### 知识点

- a[i] 是语法糖：标准定义 `a[i] ≡ *(a + i)`
  - parse 直接脱糖成 解引用 (加法 (a, i)) — 代码生成器根本没有"数组下标"概念
  - 加法里的缩放 (×4) 由类型驱动：Lesson 89 的指针算术提前在这里登场
- 数组名的"退化" (decay)
  - `a` 的类型是 `int[10]`, 但用在表达式里自动变成 `int*` (指向首元素)
  - 实现：load_val 遇到数组类型**不加载**, 地址本身就是值
  - 三个例外不退化：sizeof(a), &a, 字符串字面量初始化 — 本编译器实现了 sizeof
- 栈上的连续内存
  - `int a[10]` 在符号表里是一个 offset + size 40 的大槽位
  - stack_size 累加 40 而不是 4 — 帧布局第一次出现"大对象"
- 没有越界检查
  - `a[10] = 1;` 编译运行都不报错，默默踩坏邻居 — C 的设计哲学与代价

### 课堂讨论

- `3[a]` 居然合法且等于 `a[3]` — 用脱糖规则解释为什么
- `sizeof(a)` 是 40, 传参后在被调函数里 `sizeof(p)` 是 4 — "数组传参退化"坑了多少人？
- 越界写 `a[10]` 在本编译器的帧布局下会踩到谁？(画帧图：ra 在帧底，安全了吗？)

### 课后练习

- 测试：`int a[5]; a[0]=1; a[4]=5; return a[0]+a[4];` 应返回 6
- 测试：用循环填充 a[i]=i\*i, 求和验证
- 挑战：`int a[600];` 帧超 2400 字节 — addi 立即数 12 位会溢出，看 `addr_of_local()` 的 t6 中转路径如何救场 (Lesson 95 测试工程的真实 bug)

### 参考资料

- N1256 6.3.2.1p3 — Array decay 的三个例外
- Expert C Programming (《C 专家编程》) 第 4 章 — 数组与指针并不相同

---

### 本课文件

    test_array.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/87_arrays/test_array.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-87.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-87.md) — 如有更新以书仓为准
