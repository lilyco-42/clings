## Lesson 28 - math calculator (数学计算器实现)

### 课程任务

实现支持四则运算、括号和数学函数的表达式计算器，使用**递归下降**方法正确处理运算符优先级。

- 运行示例

      NCCL# math "2+3*4"
      14
      NCCL# math "(1+2)*(3+4)"
      21
      NCCL# math "sqrt(16)+sin(0)"
      4

### 代码

    /*
     * 递归下降解析器 - 文法:
     *   expr   = term (('+' | '-') term)*
     *   term   = factor (('*' | '/') factor)*
     *   factor = '-' factor | '(' expr ')' | number | func '(' expr ')'
     */
    static double parse_expr(void)
    {
        double val = parse_term();
        while (*pos == '+' || *pos == '-') {
            char op = *pos++;
            double right = parse_term();
            if (op == '+') val += right;
            else val -= right;
        }
        return val;
    }

### 重要知识点

- 递归下降解析 (Recursive Descent Parsing)
  - 每个语法规则对应一个函数：`parse_expr` → `parse_term` → `parse_factor`
  - 运算符优先级通过调用层次自然体现：`*`/`/` 在 `term` 层，`+`/`-` 在 `expr` 层
  - 这个方法将直接在 Unit 4 编译器中复用
- 数学函数调用
  - `parse_factor` 中识别函数名 (sin/cos/sqrt/log/exp)
  - 用 `strcmp` 分派到对应的 `<math.h>` 函数
- 错误处理
  - 除零检测：`if (right == 0) error("division by zero")`
  - 未知函数：报错并返回 0

### 课堂讨论

- 为什么 `*` 和 `/` 的优先级比 `+` 和 `-` 高？在代码中如何体现？
- 如果要添加 `^` (幂运算，右结合)，应该放在哪一层？
- 这个解析器和 Lesson 35 的栈求值法有什么区别？

### 课后练习

- 测试：`math "-(-5)"` 应返回 5
- 添加 `%` (取余) 运算符支持
- 挑战：添加变量赋值 `x=3; x*x+1`

### 参考资料

- K&R《C 程序设计语言》第 4 章 递归
- Standard C Math Library https://ganquan.info/standard-c/
- 运算符优先级表 https://en.cppreference.com/w/c/language/operator_precedence
