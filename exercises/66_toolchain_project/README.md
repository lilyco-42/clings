## Lesson 66 Project: Assemble & Run on QEMU 汇编到运行完整流程

### 课程任务
* 整合 Lesson 62-65 的工具，实现完整的"汇编源码 → 机器码 → ELF → QEMU 执行"流水线
* 用自制工具链汇编并运行 Lesson 61 的所有范例程序
* 与 GNU 工具链的输出进行对比验证

### 完整流程
	# 我们的工具链
	./rv_asm  input.s -o code.bin     # 汇编 → 二进制
	./elf_gen code.bin -o program     # 二进制 → ELF
	qemu-riscv32 ./program            # 执行
	
	# GNU 工具链 (对比验证)
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static input.s -o program_gnu
	qemu-riscv32 ./program_gnu

### 知识点
* 工具链集成
	- Makefile 自动化构建流程
	- 管道化: 前一工具输出是后一工具输入
* 验证方法
	- 对比自制工具与 GNU 工具的输出二进制
	- 反汇编对比: `rv_disasm` vs `objdump -d`
	- 执行结果对比: 两者退出码应相同
* 调试技巧
	- `hexdump -C` 对比二进制文件差异
	- `readelf -a` 检查 ELF 结构正确性

### 课堂讨论
* 我们的工具链和 GNU 工具链的输出完全一致吗？有哪些差异是可接受的？
* 如果汇编器输出有 bug，最快的定位方式是什么？
* 这个工具链在 Unit 4 中将如何被复用？

### 课后练习
* 编写一个 `run.sh` 脚本，自动完成"汇编→ELF→运行→验证"全流程
* 为工具链增加错误处理: 如果某一步失败，报告错误并终止
* 用自制工具链汇编运行一个稍复杂的程序: 计算两数最大公约数

### 参考资料
* GNU Binutils 文档 https://sourceware.org/binutils/docs/
* QEMU User Mode 文档 https://www.qemu.org/docs/master/user/main.html
