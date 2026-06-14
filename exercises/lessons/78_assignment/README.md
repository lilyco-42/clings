## Lesson 78 Assignment Expressions 赋值表达式

### 代码
	/* 输入 */
	int main() { int a; a = 10; a = a * 2; return a; }

	/* 赋值的代码生成: 先地址, 后值, 再存储 */
	    addi a0, s0, -4      # 左边: &a (gen_addr, 地址!)
	    addi sp, sp, -4      # push 地址
	    sw a0, 0(sp)
	    li a0, 10            # 右边: 求值 10
	    lw t0, 0(sp)         # pop 地址到 t0
	    addi sp, sp, 4
	    sw a0, 0(t0)         # *(&a) = 10, a0 仍保留 10 (赋值是表达式!)

### 知识点
* 赋值是表达式, 不是语句
	- `a = b = 10` 合法: `=` 右结合, 求值结果是被赋的值
	- 代码生成后 a0 保留右值, 链式赋值自然成立
* 左值 (lvalue) 与右值 (rvalue) 的分流
	- `gen_expr()` 算出"值在 a0"; `gen_addr()` 算出"地址在 a0"
	- 赋值左边必须走 `gen_addr()` — 对 `5 = x` 这类非左值直接报错
	- 这是编译器里第一次出现"同一个语法节点按上下文生成不同代码"
* 表达式语句与代码块
	- `expr;` 求值后丢弃结果 (a0 被下一条覆盖)
	- `{ s1 s2 ... }` 解析为语句链表, 顺序生成
* 声明与赋值的区别
	- `int a = 10;` 是**声明+初始化** (ND_VAR_DECL)
	- `a = 10;` 是**赋值表达式** (ND_ASSIGN) — 语法路径完全不同

### 课堂讨论
* `a = b = c = 1;` 生成几次 store？画出它的 AST
* 为什么赋值要"先 push 地址再求右值", 而不是"先求右值再算地址"？
  提示: Lesson 89 之后左边可能是 `*(p + i++)`
* C 语言为什么把赋值设计成表达式？`if (a = 0)` 的坑由此而来, 值得吗？

### 课后练习
* 测试: `int a; int b; a = b = 7; return a + b;` 应返回 14
* 测试: `return 5 = 3;` 编译器应报错 "not an lvalue"
* 里程碑自查: 至此 `int main() { ... }` 内的纯算术程序已全部可编译 — 这是 M1 里程碑

### 参考资料
* N1256 6.5.16 — Assignment operators: "An assignment expression has the value of the left operand after the assignment"

---

### 本课文件
	test_assign.c

### 在本仓验证
	cd ../../nccl-cc && make
	./nccl-cc ../lessons/78_assignment/test_assign.c > /tmp/t.s
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
	qemu-riscv32 /tmp/t; echo $?

> 课文源: [NCCL/Unit-4/Lesson-78.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-78.md) — 如有更新以书仓为准
