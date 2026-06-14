## Lesson 72 Project: Mini Language 迷你语言

### 课程任务
* 整合 Lesson 67-71，构建一个完整的迷你语言处理系统
* 支持: 变量、算术、比较、if/else、while、函数定义与调用、递归
* 实现 REPL 交互模式和文件执行模式

### 完整管线
	源代码 (.ml)
	    │
	    ▼ tokenize()
	Token 流
	    │
	    ▼ parse()
	AST (抽象语法树)
	    │
	    ▼ eval() / print_ast()
	执行结果 / 树形输出

### 验证程序
	// test_fact.ml - 阶乘
	int fact(int n) {
	    if (n <= 1) return 1;
	    return n * fact(n - 1);
	}
	int main() { return fact(5); }
	// 期望输出: 120
	
	// test_fib.ml - 斐波那契
	int fib(int n) {
	    if (n <= 1) return n;
	    return fib(n-1) + fib(n-2);
	}
	int main() { return fib(10); }
	// 期望输出: 55

### 知识点
* 完整语言前端
	- Tokenizer → Parser → AST → Interpreter 四阶段管线
	- 每阶段可独立测试 (打印中间结果)
* 从解释器到编译器的转变
	- 本课的 `eval()` 直接执行
	- Unit 4 将用 `codegen()` 替换 `eval()`，生成 RISC-V 汇编
	- AST 不变，只变后端 — 这就是编译器的核心思想
* 项目组织
	- 多文件编译: tokenizer.c, parser.c, interpreter.c, main.c
	- 头文件: 共享数据结构定义
	- Makefile: 管理编译依赖

### 课堂讨论
* 本课的 minilang 和 Unit 4 的 nccl-cc 编译器有多少代码可以复用？
* 如果要增加一种新的语句类型 (如 switch/case)，需要修改哪些文件？
* "先写解释器，再改为编译器" 这个策略的优缺点是什么？

### 课后练习
* 增加 for 循环的支持
* 增加指针的基本支持 (`&` 和 `*`)
* 将解释器改为字节码编译器 + VM 执行 (结合 Lesson 70)

### 参考资料
* "Building a compiler can be as easy as building an interpreter" — Ghuloum
* chibicc 完整实现 https://github.com/rui314/chibicc
