## Lesson 70 Stack Virtual Machine 栈虚拟机

### 代码
	/* stack_vm.c - 基于栈的虚拟机 */
	enum opcode {
	    OP_PUSH, OP_POP, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_MOD,
	    OP_EQ, OP_NE, OP_LT, OP_LE, OP_GT, OP_GE,
	    OP_JMP, OP_JZ, OP_CALL, OP_RET,
	    OP_LOAD, OP_STORE, OP_HALT
	};
	int stack[256];  /* 求值栈 */
	int sp = -1;    /* 栈顶指针 */

### 知识点
* 栈式虚拟机设计
	- 求值栈: 所有运算从栈顶取操作数，结果压回栈
	- 指令格式: opcode [operand]
	- 与寄存器机的区别: 无需寄存器分配，生成代码更简单
* 指令集
	- 数据: PUSH value, POP, LOAD addr, STORE addr
	- 算术: ADD, SUB, MUL, DIV, MOD
	- 比较: EQ, NE, LT, LE, GT, GE
	- 跳转: JMP addr, JZ addr (条件跳转)
	- 函数: CALL addr, RET
	- 控制: HALT
* 执行循环 (Fetch-Decode-Execute)
	- PC 指向当前指令
	- 读取 opcode → 执行对应操作 → PC 前进

### 课堂讨论
* 栈虚拟机 vs 寄存器虚拟机，各自的优缺点？
* Java (JVM) 和 Python 使用哪种虚拟机？为什么？
* 为什么实际 CPU (如 RISC-V) 用寄存器而非栈？

### 课后练习
* 为 VM 增加 PRINT 指令 (输出栈顶值)
* 实现一个简单的汇编器，将文本指令转为字节码数组
* 用 VM 手写字节码实现斐波那契数列

### 参考资料
* Crafting Interpreters - A Virtual Machine https://craftinginterpreters.com/a-virtual-machine.html
