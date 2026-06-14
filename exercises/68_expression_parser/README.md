## Lesson 68 Expression Parser 表达式解析器

### 代码
	/* expr_parser.c - 递归下降 + 优先级爬升解析表达式 */
	/* 输入: Token 流 (来自 Lesson 67 的 tokenizer) */
	/* 输出: 表达式 AST */
	Node *parse_expr(void);           /* 入口: 解析完整表达式 */
	Node *parse_binary(int min_prec); /* 优先级爬升 */
	Node *parse_unary(void);          /* 一元: -, !, ~ */
	Node *parse_primary(void);        /* 原子: 数字, 标识符, (expr) */

### 知识点
* 运算符优先级与结合性
	- 14 级优先级 (从赋值到一元)
	- 左结合: +, -, *, /, <, >, ==, !=, &&, ||
	- 右结合: = (赋值)
* 优先级爬升算法 (Precedence Climbing)
	- 比 Pratt Parser 更直观的表达式解析方法
	- 核心: `parse_binary(min_prec)` 只处理优先级 >= min_prec 的运算符
* AST 节点类型
	- ND_NUM: 数字字面量
	- ND_IDENT: 变量引用
	- ND_ADD/SUB/MUL/DIV: 二元算术
	- ND_EQ/NE/LT/LE: 比较
	- ND_NEG/NOT: 一元运算

### 课堂讨论
* `a + b * c` 如何通过优先级爬升得到正确的 AST？画出解析过程
* `a = b = c` 为什么需要右结合？如果左结合会怎样？
* 与 Lesson 35 (栈式表达式求值) 相比，AST 方式有什么优势？

### 课后练习
* 增加三元运算符 `? :` 的支持
* 增加数组下标 `a[i]` 的解析
* 增加函数调用 `f(a, b)` 的解析

### 参考资料
* Pratt Parsing https://matklad.github.io/2020/04/13/simple-but-powerful-pratt-parsing.html
* Precedence Climbing https://eli.thegreenplace.net/2012/08/02/parsing-expressions-by-precedence-climbing
