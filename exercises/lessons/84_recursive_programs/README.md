## Lesson 84 Project: Compile Recursive Programs 项目课: 编译递归程序

### 代码
	/* factorial: 编译器自举之路的第一块试金石 */
	int fact(int n) {
	    if (n <= 1) return 1;
	    return n * fact(n - 1);
	}
	int main() { return fact(5); }    /* 期望 120 */

	/* fibonacci: 双递归, 调用树呈指数展开 */
	int fib(int n) {
	    if (n <= 1) return n;
	    return fib(n - 1) + fib(n - 2);
	}
	int main() { return fib(10); }    /* 期望 55 */

### 知识点
* 递归能跑 = 三课成果的乘法
	- Lesson 77 栈帧: 每层调用独立的 n, 互不踩踏
	- Lesson 82 ra 保存: 嵌套调用链能一层层返回
	- Lesson 83 参数落栈: a0 被下层覆盖前已安全
	- **没写一行"支持递归"的代码, 递归自己就通了** — 机制正交的红利
* fib(n-1) + fib(n-2) 的隐藏陷阱
	- 左调用的返回值在 a0, 右调用一执行 a0 必被覆盖
	- 栈式求值的 push/pop 恰好救场: 左结果先压栈, 右调用完再弹出相加
	- 若用"寄存器缓存左值"的优化, 这里就是 caller-saved 寄存器的修罗场
* 测试驱动: // expect 头
	- 每个测试文件首行 `// expect: 120`
	- runner 编译→汇编→qemu 运行→比对退出码, 全自动回归

### 课堂讨论
* fib(10) 一共发生多少次函数调用？栈最深多少层？算给定 n 的通式
* fact(13) = 6227020800 溢出 int — 编译器该管吗？运行时该管吗？C 选择了什么？
* 尾递归 `return fact_acc(n-1, acc*n);` 可以优化成循环 — 本编译器的输出里看得到这个机会吗？

### 课后练习
* 测试: Ackermann 函数 A(2,3)=9 — 递归深度的极限测试
* 测试: 互递归 is_even/is_odd 判断奇偶
* 里程碑自查: M3 达成 — 算术 + 控制流 + 函数 + 递归, 已是图灵完备的语言核心

### 参考资料
* SICP 1.2 — Procedures and the Processes They Generate (递归与迭代过程)
* Ghuloum, "An Incremental Approach to Compiler Construction" — 本课程方法论源头

---

### 本课文件
	test_recurse.c

### 在本仓验证
	cd ../../nccl-cc && make
	./nccl-cc ../lessons/84_recursive_programs/test_recurse.c > /tmp/t.s
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
	qemu-riscv32 /tmp/t; echo $?

> 课文源: [NCCL/Unit-4/Lesson-84.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-84.md) — 如有更新以书仓为准
