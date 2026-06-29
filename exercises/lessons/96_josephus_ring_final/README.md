## Lesson 96 Final Project: Josephus Ring 终极项目：约瑟夫环

### 代码

    /* Lesson 10 我们用 gcc 编译它; 今天, 用自己写的编译器编译它 */
    int main() {
        int a[41];
        int n = 41; int m = 3;
        int i; int count; int remain;
        for (i = 0; i < n; i = i + 1) a[i] = 1;
        count = 0; i = -1; remain = n;
        while (remain > 1) {
            i = (i + 1) % n;
            if (a[i] == 0) continue;
            count = count + 1;
            if (count == m) {
                a[i] = 0;
                remain = remain - 1;
                count = 0;
            }
        }
        for (i = 0; i < n; i = i + 1)
            if (a[i] == 1) return i;
        return -1;
    }

    # 五个运行环境, 同一个答案: 30
    ./nccl-cc test/test_josephus.c > j.s && riscv64-linux-gnu-gcc \
        -march=rv32im -mabi=ilp32 -nostdlib -static \
        j.s runtime_rv_io.s runtime_rv.s -o j
    qemu-riscv32 ./j; echo $?                    # => 30
    ./nccl-cc -target=arm ... && qemu-arm ...     # => 30
    ./nccl-cc -target=a64 ... && qemu-aarch64 ... # => 30
    ./nccl-cc -target=x64 ... && ./j              # => 30 (本机直跑)
    # 裸机: 同一份 j.s + 裸机平台层 + 链接脚本, 跑在没有 OS 的虚拟板上
    riscv64-linux-gnu-gcc ... -T bare_rv.ld j.s runtime_rv_io.s runtime_rv_bare.s -o j.elf
    qemu-system-riscv32 -machine virt -nographic -bios none -kernel j.elf; echo $?  # => 30

### 知识点

- 课程闭环：从用编译器到造编译器
  - Lesson 10 的约瑟夫环一行未改 — 数组/取模/continue/嵌套 if 全部接得住
  - 用 24 课写的 ~4000 行 C, 编译 96 课前的自己 — "理解一个东西的最好方式是造一个"
- 裸机运行：没有操作系统的世界 (进阶)
  - putchar 不再是系统调用，而是**写 UART 设备寄存器** (0x10000000 轮询发送)
  - exit 不存在 — 向 qemu 的 test finisher 设备 (0x100000) 写状态请求模拟器退出
  - 链接脚本把 \_start 钉在 0x80000000 (板子上电跳转地址), 栈指针自己拉起
  - runtime 分层的回报 (Lesson 93): io 层零改动，只换 ~40 行平台层
- 一棵 AST, 四个后端，五个运行环境
  - ILP32 (rv32/arm) 与 LP64 (a64/x64) 两种数据模型并存 — 指针宽度由 target 决定
  - 36 个正向用例 + 4 个负向用例 × 5 路全绿; 差分模糊 900+ 种子与 gcc 一致
- 下一站：操作系统
  - 裸机课已替 OS 课程铺好路：MMIO、链接脚本、中断前夜 — xv6 见

### 课堂讨论

- 这个编译器离"自举" (编译自己) 还差什么特性？列清单，排优先级
- 五个环境同一退出码 — 这能证明编译器"正确"吗？测试与证明的边界在哪里？
- 如果只允许保留三个测试用例守护这个编译器，你选哪三个？为什么？

### 课后练习

- 终极验证：`make test-all && make fuzz` 全绿 — 然后给 josephus 换 n=100, m=7
- 把你在 Unit 1-3 写过的任一程序 (规则内) 喂给 nccl-cc — 能编译吗？不能的话缺什么？
- 毕业挑战：给编译器加一个你最想要的特性 (块作用域 / -> 运算符 / 带参宏), 带测试提 PR

### 参考资料

- chibicc — 课程的精神源头，继续读它的 struct/enum/typedef 实现
- xv6: a simple Unix-like teaching OS — 裸机之后的下一座山
- 《Crafting Interpreters》— 另一条路线：字节码虚拟机

---

### 本课文件

    test_josephus.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/96_josephus_ring_final/test_josephus.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-96.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-96.md) — 如有更新以书仓为准
