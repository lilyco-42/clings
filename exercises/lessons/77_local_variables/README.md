## Lesson 77 Local Variables 局部变量

### 代码
	/* 输入 */
	int main() { int a = 5; int b = a + 3; return b; }

	/* 输出: 栈帧 + s0 偏移寻址 */
	main:
	    addi sp, sp, -32     # 分配栈帧 (locals 16B 对齐 + ra/s0 16B)
	    sw ra, 0(sp)         # 返回地址存帧底
	    sw s0, 4(sp)         # 旧帧指针存帧底
	    addi s0, sp, 32      # s0 指向帧顶, 局部变量用 s0 负偏移
	    li a0, 5
	    sw a0, -4(s0)        # a = 5      (a 在 s0-4)
	    addi a0, s0, -4      # &a
	    lw a0, 0(a0)         # 读 a
	    addi sp, sp, -4      # push a
	    sw a0, 0(sp)
	    li a0, 3
	    lw t0, 0(sp)         # pop
	    addi sp, sp, 4
	    add a0, a0, t0       # a + 3
	    sw a0, -8(s0)        # b = 8      (b 在 s0-8)
	    addi a0, s0, -8
	    lw a0, 0(a0)         # 读 b
	    j .L_return_main
	.L_return_main:
	    lw ra, 0(sp)
	    lw s0, 4(sp)
	    addi sp, sp, 32      # 释放栈帧
	    ret

### 知识点
* 符号表: 名字 → 栈偏移
	- parse 阶段为每个 `int x` 分配递增偏移 (4 字节一个槽)
	- Var 链表记录 name/offset, 同名变量直接复用同一槽位
	- 代码生成时变量读写一律 `±offset(s0)`, 不再关心名字
* 栈帧布局 (高地址 → 低地址)
	- `[s0-4 .. s0-N]` 局部变量区, 向下生长
	- `[sp+4]` 保存的 s0, `[sp+0]` 保存的 ra — 存在**帧底**
	- s0 (帧指针) 在函数体内不变, 是所有局部变量的锚点
* 为什么需要帧指针 s0
	- sp 在表达式求值时会临时变化 (Lesson 75 的 push/pop)
	- 用会动的 sp 寻址变量, 偏移就要随时修正; s0 不动, 偏移恒定
* 变量地址与变量值
	- `addi a0, s0, -4` 得到的是**地址** (左值)
	- `lw a0, 0(a0)` 才取出**值** (右值) — 为 Lesson 88 指针埋下伏笔

### 课堂讨论
* 把 ra 保存在帧顶 (紧贴 s0) 会发生什么？提示: 局部变量从 s0 向下生长, 第一个变量在 s0-4
* 为什么栈帧大小要对齐到 16 字节？RISC-V psABI 怎么规定的？
* 两个 `int a` 声明同一个名字, 本编译器如何处理？块级作用域呢？

### 课后练习
* 测试: `int x = 1; int y = 2; int z = 3; return x + y + z;` 应返回 6
* 用 `-dump-ast` 观察每个变量节点标注的 `[fp-N]` 偏移
* 挑战: 声明 600 个 int 的数组后 (Lesson 87), `addi` 的 12 位立即数会溢出 — 看看 `addr_of_local()` 如何处理 (提示: 经 t6 中转)

### 参考资料
* RISC-V psABI — Frame Pointer Convention
* chibicc step "Add local variables" https://github.com/rui314/chibicc

---

### 本课文件
	test_var.c

### 在本仓验证
	cd ../../nccl-cc && make
	./nccl-cc ../lessons/77_local_variables/test_var.c > /tmp/t.s
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
	qemu-riscv32 /tmp/t; echo $?

> 课文源: [NCCL/Unit-4/Lesson-77.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-77.md) — 如有更新以书仓为准
