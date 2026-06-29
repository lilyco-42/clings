## Lesson 79 If/Else 条件分支

### 代码

    /* 输入 */
    int main() { int a = 1; if (a > 0) { return 1; } else { return 2; } }

    /* 输出: 条件为假跳 else, 真分支结尾跳过 else */
        ...                  # a0 = (a > 0) 的结果 0/1
        beqz a0, .L_else_0   # 条件为 0 → else 分支
        li a0, 1             # then 分支
        j .L_return_main
        j .L_endif_0         # then 结束, 跳过 else
    .L_else_0:
        li a0, 2             # else 分支
        j .L_return_main
    .L_endif_0:

### 知识点

- 结构化控制流 → 跳转指令的固定翻译模式
  - `if (C) T` → `C; beqz .L_endif; T; .L_endif:`
  - `if (C) T else E` → `C; beqz .L_else; T; j .L_endif; .L_else: E; .L_endif:`
  - 高级语言的嵌套结构，在汇编层全部摊平成标号 + 跳转
- 标号唯一性
  - 全局计数器 `label_count++`, 每个 if 拿到独立编号
  - 嵌套 if 互不干扰 — 递归生成时编号天然唯一
- beqz: 分支指令只看 0 / 非 0
  - Lesson 76 的比较结果就是 0/1, 与 beqz 无缝衔接
  - `if (x)` 不需要比较指令 — x 本身当条件

### 课堂讨论

- `else` 悬挂问题：`if (a) if (b) s1; else s2;` — else 跟谁？递归下降解析器如何"天然"解决？
- then 分支以 `return` 结尾时，后面的 `j .L_endif_0` 永远不会执行 — 编译器要不要删掉它？(死代码消除)
- 为什么用 `beqz`(跳过) 而非 `bnez`(跳入)？两种翻译模式画图对比

### 课后练习

- 测试：三层嵌套 if-else, 用 `-dump-ast` 验证树形结构
- 测试：`if (0) return 1; return 2;` 应返回 2
- 扩展：支持不带大括号的单语句分支 `if (a) return 1;`

### 参考资料

- 龙书 6.6 — Control Flow translation

---

### 本课文件

    test_if.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/79_if_else/test_if.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-79.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-79.md) — 如有更新以书仓为准
