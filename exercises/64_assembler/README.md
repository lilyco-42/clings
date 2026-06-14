## Lesson 64 Simple Assembler 简易汇编器

### 代码
	/* rv_asm.c - 两遍 RISC-V 汇编器 */
	/* Pass 1: 收集标号地址，构建符号表 */
	/* Pass 2: 编码指令，解析前向引用 */

### 知识点
* 两遍汇编
	- 第一遍: 扫描所有标号，记录地址到符号表
	- 第二遍: 逐条编码指令，标号引用查符号表解析
* 符号表
	- 标号名 → 地址 的映射
	- 前向引用问题: 标号在使用之后才定义
* 伪指令展开
	- `li rd, imm` → `lui` + `addi` (大立即数) 或单 `addi` (小立即数)
	- `call func` → `auipc ra, %hi(off)` + `jalr ra, %lo(off)(ra)`
	- `ret` → `jalr zero, 0(ra)`
	- `mv rd, rs` → `addi rd, rs, 0`
* 指令解析
	- 分割助记符和操作数
	- 寄存器名解析 (复用 Lesson 62 的 reg_num)
	- 立即数解析 (十进制/十六进制)
	- 偏移量寻址: `offset(reg)` 格式

### 课堂讨论
* 为什么需要两遍扫描？能不能一遍完成？
* 如果两个标号重名怎么办？
* 伪指令 `li rd, 0x12345678` 展开后需要几条指令？

### 课后练习
* 用本课汇编器汇编 Lesson 61 的 return42.s，输出二进制，与 GNU as 对比
* 增加 `.globl`, `.text`, `.data`, `.word` 伪指令的支持
* 增加简单的错误报告: 未知指令、未定义标号

### 参考资料
* GNU as RISC-V 汇编手册 https://sourceware.org/binutils/docs/as/RISC_002dV_002dFormats.html
