## Lesson 69 AST Builder 抽象语法树构建

### 代码
	/* ast_builder.c - 完整的语句级 AST 构建 */
	/* 在 Lesson 68 表达式解析基础上增加语句和声明解析 */
	Node *parse_stmt(void);       /* if/while/for/return/block/expr_stmt */
	Node *parse_compound(void);   /* { stmt* } */
	Node *parse_declaration(void); /* int name [= expr]; */
	Node *parse_function(void);   /* type name(params) { body } */
	void print_ast(Node *node, int indent); /* 树形打印 AST */

### 知识点
* 语句节点类型
	- ND_IF: 条件分支 (cond, then, els)
	- ND_WHILE: 循环 (cond, body)
	- ND_FOR: for循环 (init, cond, inc, body)
	- ND_RETURN: 返回语句 (expr)
	- ND_BLOCK: 复合语句 (stmt_list)
	- ND_EXPR_STMT: 表达式语句
* 声明节点
	- ND_VAR_DECL: 变量声明 (type, name, init_expr)
	- ND_FUNC: 函数定义 (return_type, name, params, body)
* AST 打印与可视化
	- 缩进式树形输出，方便调试
	- 验证解析结果是否符合预期

### 课堂讨论
* `if (a) if (b) x; else y;` — else 属于哪个 if？(悬挂 else 问题)
* for 循环 `for(int i=0; i<n; i++)` 能否转换为 while？怎么在 AST 中表示？
* 为什么要把 AST 打印出来？在编译器开发中这有多重要？

### 课后练习
* 增加 do-while 语句的解析
* 增加 break/continue 的解析
* 实现 AST 的 JSON 格式输出 (方便可视化工具读取)

### 参考资料
* chibicc parse.c https://github.com/rui314/chibicc/blob/main/parse.c
