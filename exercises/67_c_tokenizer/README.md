## Lesson 67 C Subset Tokenizer C子集词法分析器

### 代码
	/* tokenizer.c - 手写 C 子集词法分析器 */
	enum token_kind {
	    TK_NUM, TK_IDENT, TK_KEYWORD, TK_PUNCT, TK_STRING, TK_EOF
	};
	struct token {
	    int kind;
	    int val;           /* TK_NUM 的数值 */
	    char *str;         /* 标识符/关键字/字符串的文本 */
	    int len;           /* 文本长度 */
	    char *loc;         /* 在源码中的位置 (用于错误报告) */
	    int line;          /* 行号 */
	};

### 知识点
* Token 类型设计
	- TK_NUM: 整数字面量 (十进制)
	- TK_IDENT: 标识符 (变量名/函数名)
	- TK_KEYWORD: 关键字 (int, char, if, else, while, for, return)
	- TK_PUNCT: 标点/运算符 (+, -, *, /, ==, !=, <=, >=, &&, ||, ...)
	- TK_STRING: 字符串字面量
	- TK_EOF: 文件结束
* 手写 Tokenizer vs Lesson 21 的 flex 表驱动
	- 手写: 更灵活，更易扩展，更好的错误信息
	- flex: 更通用，适合复杂正则规则
* 错误报告
	- 记录每个 Token 的行号和源码位置
	- 出错时能指出具体位置

### 课堂讨论
* 为什么不继续使用 Lesson 21 的 flex 表驱动方式？
* `>=` 和 `> =`(带空格) 应该被解析为什么？如何处理多字符运算符？
* 如何区分关键字 `int` 和标识符 `integer`？

### 课后练习
* 增加对十六进制字面量 (0xFF) 的支持
* 增加对字符字面量 ('A', '\n') 的支持
* 增加对 C 风格注释 (/* */ 和 //) 的跳过

### 参考资料
* chibicc tokenize.c https://github.com/rui314/chibicc/blob/main/tokenize.c
