## Lesson 80 While & For Loops 循环语句

### 代码

    /* 输入 */
    int main() { int s = 0; int i; for (i = 1; i <= 100; i = i + 1) s = s + i; return s; }

    /* for 的输出骨架 */
        ...                  # init: i = 1
    .L_for_0:
        ...                  # cond: i <= 100 → a0
        beqz a0, .L_fend_0   # 不满足 → 出循环
        ...                  # body: s = s + i
    .L_cont_0:
        ...                  # step: i = i + 1
        j .L_for_0
    .L_fend_0:

    /* while 的输出骨架 */
    .L_while_1:
        ...                  # cond
        beqz a0, .L_wend_1
        ...                  # body
        j .L_while_1
    .L_wend_1:

### 知识点

- 循环 = "回跳"的条件分支
  - while: 条件在头部，先判再做，0 次起步
  - for 比 while 多 init 与 step 两个挂载点，本质同构
- for 到 while 的等价变换
  - `for (I; C; S) B` ≡ `I; while (C) { B; S; }`
  - 但 step 必须有独立标号 `.L_cont_N` — `continue` 要跳到 step 而非条件 (Lesson 81)
- AST 节点设计
  - ND_FOR 挂 init/cond/inc/body 四个孩子，任何一个都可为空
  - `for (;;)` 是合法的死循环：cond 为空时不生成 beqz
- 1+2+...+100 = 5050 > 255
  - 进程退出码只有 8 位！`echo $?` 看到的是 5050 & 255 = 186
  - 测试期望值必须按 `& 255` 折算 — 测试框架的真实约束

### 课堂讨论

- 把条件判断放循环尾部 (do-while 形态) 可以省一条 `j`, 编译器值得这么优化吗？
- `for (int i = 0; ...)` 的 i 在 C99 里属于哪个作用域？本编译器如何处理？
- 退出码只有 8 位，那如何验证 5050 这种大结果？(提示：Lesson 93 printf)

### 课后练习

- 测试：双重循环打印 9×9 口诀表的"次数": `return 81 & 255;`
- 测试：`while (0) s = 99; return s;` — 体会 0 次循环
- 用 `qemu-riscv32` 跑 1 到 100 求和，`echo $?` 验证 186

### 参考资料

- 龙书 6.7 — Backpatching (一遍生成跳转的另一种思路)

---

### 本课文件

    test_for.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/80_while_for/test_for.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-80.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-80.md) — 如有更新以书仓为准
