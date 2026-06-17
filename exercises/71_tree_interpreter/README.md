## Lesson 71 Tree-Walk Interpreter 树遍历解释器

### 代码

    /* interpreter.c - 遍历 AST 直接执行 */
    /* 整合 Lesson 67 (tokenizer) + Lesson 68-69 (parser/AST) */
    int eval(Node *node, Env *env);  /* 核心: 递归遍历 AST 求值 */

    /* 环境 (变量作用域) */
    typedef struct Env {
        char *names[64];
        int values[64];
        int count;
        struct Env *parent;  /* 外层作用域 */
    } Env;

### 知识点

- 树遍历解释器 (Tree-Walk Interpreter)
  - 直接递归遍历 AST 节点
  - 遇到表达式：计算并返回值
  - 遇到语句：执行副作用 (赋值/打印/跳转)
- 环境与作用域
  - 环境：变量名 → 值 的映射表
  - 嵌套作用域：函数内新建子环境，查找时沿 parent 链上溯
  - 函数调用：创建新环境帧，参数绑定到形参名
- 控制流实现
  - if: 求值条件，选择 then/else 分支执行
  - while: 循环求值条件 + 执行 body
  - return: 用 longjmp 或特殊返回值跳出函数

### 课堂讨论

- 解释器和编译器的核心区别是什么？(提示：何时知道变量的值？)
- 如果我们把 `eval()` 中的"计算值"替换为"生成汇编代码"，得到的是什么？
- 递归解释器的最大问题是什么？(提示：深层递归)

### 课后练习

- 增加数组支持：`int a[5]; a[2] = 42;`
- 增加简单的 `printf` 支持 (仅 `%d`)
- 对比：用解释器运行 fact(5) 和 Lesson 61 的 RISC-V 汇编版本

### 参考资料

- Crafting Interpreters - A Tree-Walk Interpreter https://craftinginterpreters.com/a-tree-walk-interpreter.html
