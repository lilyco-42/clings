## Lesson 86 Char Type & Strings 字符与字符串

### 代码
	/* 输入 */
	int main() { char *s = "hi"; char c = s[0]; return c; }    /* 'h' = 104 */

	/* 字符串字面量进只读段 */
	.section .rodata
	.L_str_0:
	    .string "hi"         # 自动补 NUL 结尾

	/* char 的读写换成字节指令 */
	    la a0, .L_str_0      # 字符串的值 = 它的地址
	    ...
	    lb a0, 0(a0)         # 按字节读, 符号扩展到 32 位
	    sb a0, -1(s0)        # 按字节写 (char 槽只占 1 字节)

### 知识点
* 类型第一次产生"代码差异"
	- 至今 int 一统天下, lw/sw 包打一切
	- char 出现后: 读写宽度必须看类型 — lb/sb (1B) vs lw/sw (4B)
	- Type 结构体 + size 字段从本课开始驱动代码生成
* lb 是符号扩展加载
	- `char c = 0x80;` 读出来是 -128 — C 的 char 在本实现按 signed 处理
	- 无符号加载是 lbu — 对比 Lesson 97 裸机 UART 轮询为何必须用 lbu
* 字符串字面量的三个事实
	- 类型是 `char[N]`, 在表达式里退化为 `char*` (Lesson 87 详谈退化)
	- 存储在 .rodata: 只读, 写它是 UB (段错误)
	- 转义序列 `\n` `\0` `\\` 在**词法阶段**一次性解码成真实字节 — 发射 .string 时再反向转义
* 字符字面量 'A' 就是 int 65
	- tokenize 直接折成整数 token, parse/codegen 对它毫无感知

### 课堂讨论
* `char *s = "abc"; s[0] = 'x';` 会发生什么？`char s[] = "abc";` 呢？
* 为什么转义解码放词法层而不是代码生成层？两处各解一次会出什么 bug？
* "hi" 占几个字节？`sizeof("hi")` 在标准 C 里是多少？

### 课后练习
* 测试: `return "hello"[4];` 应返回 111 ('o')
* 测试: `char c = 'A' + 1; return c;` 应返回 66
* 用 `objdump -s -j .rodata` 查看字符串的十六进制存储, 找到结尾的 00

### 参考资料
* N1256 6.4.5 — String literals
* K&R 1.9 — Character Arrays

---

### 本课文件
	test_char.c

### 在本仓验证
	cd ../../nccl-cc && make
	./nccl-cc ../lessons/86_char_strings/test_char.c > /tmp/t.s
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
	qemu-riscv32 /tmp/t; echo $?

> 课文源: [NCCL/Unit-4/Lesson-86.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-86.md) — 如有更新以书仓为准
