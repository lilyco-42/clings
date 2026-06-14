## Lesson 63 Simple Disassembler 简易反汇编器

### 代码
	/* rv_disasm.c - 读取 32 位机器码，输出 RISC-V 汇编文本 */
	uint32_t inst = read_word(fp);
	int opcode = inst & 0x7F;
	int rd     = (inst >> 7) & 0x1F;
	int funct3 = (inst >> 12) & 0x7;
	int rs1    = (inst >> 15) & 0x1F;
	int rs2    = (inst >> 20) & 0x1F;
	int funct7 = (inst >> 25) & 0x7F;

### 知识点
* 指令解码
	- 从 32 位二进制中提取各字段: opcode, rd, funct3, rs1, rs2, funct7
	- 位操作: 移位和掩码
* 立即数符号扩展
	- I-type: 12 位有符号 → 32 位 (算术右移)
	- B-type: 13 位有符号 (位域重排后扩展)
	- J-type: 21 位有符号 (位域重排后扩展)
* 反汇编流程
	- 先按 opcode 判断指令类型
	- 再按 funct3/funct7 细分具体指令
	- 格式化输出助记符和操作数

### 课堂讨论
* 反汇编器和汇编器是什么关系？能否用同一套数据结构实现？
* 如何处理非法指令（无法识别的 opcode）？
* 与 `riscv64-linux-gnu-objdump -d` 的输出对比，哪些信息我们没有？

### 课后练习
* 将 Lesson 61 的 fact.s 编译为二进制，用本课的反汇编器反汇编，验证正确性
* 增加对 M 扩展指令(mul/div/rem)的支持
* 增加对伪指令的识别 (如 `ret` 实际是 `jalr x0, x1, 0`)

### 参考资料
* RISC-V 指令编码表 https://five-embeddev.com/riscv-isa-manual/latest/instr-table.html
