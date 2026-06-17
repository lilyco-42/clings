## Lesson 76 Comparison & Logic 比较与逻辑运算

### 代码

    /* 输入 */
    int main() { return (3 < 5) && (10 > 2); }

    /* 输出: 短路求值 */
    main:
        # 左: 3 < 5
        li a0, 5           # rhs
        push
        li a0, 3           # lhs
        pop t0
        slt a0, a0, t0     # 3 < 5 = 1
        beqz a0, .L_false_0  # 短路: 若左为0跳过
        # 右: 10 > 2
        li a0, 2
        push
        li a0, 10
        pop t0
        slt a0, t0, a0     # 10 > 2 = 1
        snez a0, a0
        j .L_end_0
    .L_false_0:
        li a0, 0
    .L_end_0:
        ret

### 知识点

- 比较运算的 RISC-V 实现
  - `<`: `slt` (set less than)
  - `<=`: `slt` + `xori 1` (取反 `>`)
  - `==`: `sub` + `seqz`
  - `!=`: `sub` + `snez`
- 短路求值 (Short-circuit evaluation)
  - `&&`: 左为 false 则跳过右，直接返回 0
  - `||`: 左为 true 则跳过右，直接返回 1

### 课堂讨论

- 短路求值有什么实际意义？(如 `p != NULL && *p == 0`)
- `<=` 为什么不直接用 `sle` 指令？(RISC-V 没有 sle)

### 课后练习

- 测试：`return 5 == 5;` 应返回 1
- 测试：`return 0 || 3;` 应返回 1
