## Lesson 81 Do-While, Break, Continue 循环控制

### 代码
	/* 输入 */
	int main() {
	    int x = 0;
	    do { x = x + 1; if (x == 5) break; } while (x < 10);
	    return x;
	}

	/* break/continue 跳到哪? 每层循环登记一对标号 */
	.L_do_0:
	    ...                  # body
	    j .L_brk_0           # break → 本层出口
	.L_cont_0:
	    ...                  # cond
	    bnez a0, .L_do_0     # do-while: 条件真则回跳
	.L_brk_0:

### 知识点
* do-while: 先做再判, 至少执行一次
	- 条件在尾部, 用 `bnez` 回跳 — 与 while 的 `beqz` 出循环对偶
* break/continue 的本质: 受限的 goto
	- break → 跳最近一层循环的 `.L_brk_N`
	- continue → 跳最近一层循环的 `.L_cont_N` (for 的 step / while 的 cond)
* 循环上下文栈
	- 代码生成器维护 `loop_break_labels[] / loop_cont_labels[]` 数组 + 深度计数
	- 进入循环压栈, 离开弹栈 — 嵌套循环里 break 自动找到"最近一层"
	- 在循环外使用 break → 栈空, 编译报错而非生成野跳转
* switch 也占用 break 栈位 (Lesson 91 之后)
	- break 共享, continue 不共享 — 两个数组分开管理的原因

### 课堂讨论
* `continue` 在 for 与 while 里跳的位置为什么不同？都跳条件行不行？
* 双重循环里想 break 外层 — C 给了什么办法？(goto / 标志变量 / 函数提取)
* "受限的 goto" 受了什么限制？Dijkstra 反对的是哪种 goto？

### 课后练习
* 测试: `do { ... } while (0);` — 宏定义里最常见的形态, 验证恰好执行一次
* 测试: 嵌套循环 + continue, 验证只影响内层
* 错误测试: 裸 `break;` 在函数体顶层, 编译器应报错

### 参考资料
* Dijkstra, "Go To Statement Considered Harmful" (1968)
* K&R 3.7 — Break and Continue

---

### 本课文件
	test_dowhile.c

### 在本仓验证
	cd ../../nccl-cc && make
	./nccl-cc ../lessons/81_do_while_break/test_dowhile.c > /tmp/t.s
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
	qemu-riscv32 /tmp/t; echo $?

> 课文源: [NCCL/Unit-4/Lesson-81.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-81.md) — 如有更新以书仓为准
