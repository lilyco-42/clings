## Lesson 75 Binary Arithmetic 二元算术运算

### 代码
	/* 输入 */
	int main() { return 2 + 3 * 4; }
	
	/* 输出: 栈式求值 */
	main:
	    li a0, 4          # 右操作数: 4
	    addi sp, sp, -4   # push
	    sw a0, 0(sp)
	    li a0, 3          # 左操作数: 3
	    lw t0, 0(sp)      # pop
	    addi sp, sp, 4
	    mul a0, a0, t0    # 3 * 4 = 12
	    addi sp, sp, -4   # push 12
	    sw a0, 0(sp)
	    li a0, 2          # 2
	    lw t0, 0(sp)      # pop 12
	    addi sp, sp, 4
	    add a0, a0, t0    # 2 + 12 = 14
	    ret

### 知识点
* 栈式表达式求值
	- 对于 `a OP b`: 先求右(push), 再求左, pop 右到 t0, 执行 OP
	- 递归自然处理优先级: AST 深层节点先求值
* RISC-V 算术指令
	- `add`, `sub`: 基本整数 I 扩展
	- `mul`, `div`, `rem`: M 扩展

### 课堂讨论
* 为什么先生成右操作数再生成左操作数？颠倒顺序可以吗？
* 栈式求值的效率如何？有没有更好的方式？

### 课后练习
* 测试: `return (1+2)*(3+4);` 应返回 21
* 测试: `return 100 / 3;` 应返回 33 (整数除法)
