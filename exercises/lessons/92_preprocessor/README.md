## Lesson 92 Preprocessor Integration 预处理器集成

### 代码
	/* 输入 */
	#define MAX 100
	#define DEBUG
	#include "config.h"

	#ifdef DEBUG
	int main() { return MAX; }
	#else
	int main() { return 0; }
	#endif

	/* 管线第 0 步: 纯文本变换, 发生在 tokenize 之前 */
	源文件 → preprocess() → 展开后的源码 → tokenize() → ...

### 知识点
* 预处理是"文本层"的程序
	- #define: 名字 → 替换文本的字符串表; 逐行扫描替换
	- #include: 把文件内容原地展开 — 递归调用预处理自身
	- 与 Unit-1 Lesson 20 手写的预处理器同源 — 现在它进了编译器管线
* 条件编译用"栈"管理嵌套
	- `#ifdef` 压栈当前的输出开关, `#else` 翻转, `#endif` 弹栈
	- 嵌套 ifdef 内外互不干扰; 不配对的 #endif 报错
	- 关闭区间内的行直接丢弃 — 连词法错误都不报 (这是标准行为!)
* 字符串保护: 替换不能进引号
	- `printf("MAX")` 里的 MAX 不能被替换 — 扫描时跳过字符串/字符字面量
	- 这是文本替换系统最经典的边界 bug, 测试必须覆盖
* -E 与 -D: 与 gcc 对齐的调试出口
	- `./nccl-cc -E file.c` 只输出预处理结果 — 排查宏问题第一步
	- `-D NAME=VALUE` 命令行预定义 — CI 矩阵编译的入口

### 课堂讨论
* `#define SQR(x) x*x` 后 `SQR(1+2)` 出什么结果？带参宏的坑为什么本编译器干脆不做？
* #include 自己包含自己会怎样？include guard 与 `#pragma once` 各怎么解决？
* 宏替换后行号变了 — 报错信息怎么还能指向原始行？(#line 指示与行号映射)

### 课后练习
* 测试: `#define A B` + `#define B 42` — 链式替换能展开几层？
* 测试: `#ifdef` 嵌套三层, 内层 #else 不应影响外层
* 用 `-E` 对比 `gcc -E` 的输出差异 (gcc 会插入 # linemarker 行)

### 参考资料
* N1256 6.10 — Preprocessing directives
* 《C 陷阱与缺陷》— 预处理器一章

---

### 本课文件
	test_define.c

### 在本仓验证
	cd ../../nccl-cc && make
	./nccl-cc ../lessons/92_preprocessor/test_define.c > /tmp/t.s
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
	qemu-riscv32 /tmp/t; echo $?

> 课文源: [NCCL/Unit-4/Lesson-92.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-92.md) — 如有更新以书仓为准
