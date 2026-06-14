## Lesson 65 ELF File Generator ELF文件生成器

### 代码
	/* elf_gen.c - 生成最小可执行 ELF32 文件 */
	/* 将 Lesson 64 汇编器输出的二进制包装为 ELF 可执行文件 */
	Elf32_Ehdr ehdr;  /* ELF 文件头 */
	Elf32_Phdr phdr;  /* 程序头 (LOAD 段) */
	/* 文件布局: [ELF Header][Program Header][Code] */

### 知识点
* ELF 文件结构 (对比 Lesson 26 的解析)
	- ELF Header: 魔数、类型(EXEC)、入口地址、段表偏移
	- Program Header: 类型(PT_LOAD)、文件偏移、虚拟地址、大小、权限
	- 最小 ELF: 仅需 ELF Header + 1 个 Program Header + 代码段
* 关键字段
	- `e_entry`: 程序入口地址 (代码加载位置)
	- `p_vaddr`: 段虚拟地址 (通常 0x10000 用于 RISC-V)
	- `p_filesz`/`p_memsz`: 文件中/内存中段大小
	- `p_flags`: PF_R | PF_X (可读可执行)
* 字节序
	- RISC-V 使用小端序 (Little-Endian)
	- ELF 中 `e_ident[EI_DATA] = ELFDATA2LSB`

### 课堂讨论
* ELF Header 和 Program Header 能否重叠以减小文件大小？
* 为什么 `e_entry` 不是 0？加载地址由谁决定？
* 对比 Lesson 26 解析 ELF 和本课生成 ELF，哪些字段是对称的？

### 课后练习
* 用本课生成的 ELF 在 QEMU 上运行 Lesson 64 的汇编输出
* 用 `readelf -a` 检查生成的 ELF 文件，与 GCC 生成的对比差异
* 增加 `.data` 段支持 (第二个 Program Header)

### 参考资料
* ELF 规范 https://refspecs.linuxfoundation.org/elf/elf.pdf
* 最小 ELF http://www.muppetlabs.com/~breadbox/software/tiny/teensy.html
