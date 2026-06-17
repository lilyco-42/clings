## Lesson 74 Unary Operators 一元运算

### 代码

    /* 输入 */
    int main() { return -42; }
    int main() { return ~0; }
    int main() { return !5; }

    /* 输出 (RISC-V 汇编) */
    .globl main
    main:
        li a0, 42
        neg a0, a0       # -42
        ret

### 知识点

- 一元运算符的 AST 表示
  - ND_NEG: 取反 (-)
  - ND_NOT: 逻辑非 (!)
  - ND_BITNOT: 按位取反 (~)
- RISC-V 指令对应
  - `-expr` → `neg a0, a0` (即 `sub a0, zero, a0`)
  - `~expr` → `not a0, a0` (即 `xori a0, a0, -1`)
  - `!expr` → `seqz a0, a0`
- 递归下降解析中的一元运算
  - 一元运算符优先级高于二元运算符
  - 可嵌套：`--x` 等价于 `-(-x)`

### 课堂讨论

- `neg` 和 `not` 是 RISC-V 的真指令还是伪指令？它们展开后是什么？
- 为什么 `!0` 返回 1 而 `!5` 返回 0？

### 课后练习

- 测试 `return -(-42);` 是否返回 42
- 测试 `return !!5;` 是否返回 1
