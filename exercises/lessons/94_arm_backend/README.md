## Lesson 94 ARM Backend 第二后端

### 代码
	/* 同一个 AST, 换一张"指令翻译表" */

	| 概念       | RISC-V              | ARM (ARMv7)            |
	|-----------|---------------------|------------------------|
	| 返回值/参数 | a0 / a0-a7          | r0 / r0-r3             |
	| 帧指针     | s0                  | fp (r11)               |
	| 返回地址   | ra                  | lr (r14)               |
	| 调用/返回  | call / ret          | bl / bx lr             |
	| 加载常量   | li a0, N            | movw + movt 对          |
	| 读/写内存  | lw / sw             | ldr / str              |
	| 字节读写   | lb / sb             | ldrsb / strb           |
	| 条件跳转   | beqz a0, L          | cmp r0, #0 + beq L     |
	| 比较小于   | slt a0, a0, t0      | cmp + movlt r0, #1     |

	/* codegen_arm.c 的常量材料化: 为什么不用 ldr r0, =N ? */
	    movw r0, #lower16    # 低 16 位 (高位清零)
	    movt r0, #upper16    # 高 16 位 (仅在非零时发射)

### 知识点
* 多后端架构: AST 是分水岭
	- tokenize/parse/type 完全复用 — 前端不知道后端是谁
	- codegen_arm.c 与 codegen_rv.c 平行实现同一棵树的遍历
	- `-target=arm` 只是换一个函数指针 — 这就是"中间表示"思想的最小版本
* 字面量池的教训 (本仓库的真实 bug)
	- ARM 经典写法 `ldr r0, =12345` 把常量存进代码段的"字面量池", ±4KB 内寻址
	- 长函数 + 编译器从不发射 .ltorg 转储点 → 汇编报 "pool needs to be closer"
	- 手写测试都太短从未触发; **差分模糊测试第一轮就抓到了** (Lesson 95)
	- 修复: movw/movt 直接编码 16+16 位 — 不依赖任何池
* 立即数编码: 每个 ISA 的私房规则
	- RISC-V addi: 12 位有符号 (±2047)
	- ARM 数据处理指令: 8 位值偶数循环右移 — 4096 能编码, 4095 反而不能!
	- ARM ldr/str 偏移: ±4095 — 同一架构两套规则, 大栈帧两处都要兜底
* 条件码 vs 比较指令
	- RISC-V 哲学: slt 出 0/1, 无标志寄存器
	- ARM 哲学: cmp 设 NZCV 标志, 后续指令带条件后缀 (movlt/beq)

### 课堂讨论
* AST 里完全没有"寄存器"概念 — 两个后端各自怎么决定用哪个寄存器？
* ARM 只有 4 个参数寄存器, RISC-V 有 8 个 — 同一个 5 参函数两后端怎么办？
* `qemu-arm` 与 `qemu-riscv32` 跑同一程序退出码必须一致 — 哪类 bug 会让它们不一致？(提示: 未定义行为)

### 课后练习
* 测试: `make test-arm` 全量跑通 — 与 RISC-V 同一套 // expect 头
* 对比: 同一个 test_josephus.c 两个后端的汇编行数与指令数
* 挑战: 阅读 codegen_a64.c — 第三、四后端 (AArch64/x86-64) 把 int 留在 32 位、指针放到 64 位, w/x 寄存器按类型宽度切换 (LP64 数据模型)

### 参考资料
* ARM Architecture Reference Manual — A8.8 movw/movt
* 本仓库 commit 历史: "movw/movt 取代字面量池伪指令" — 一个真实 bug 的完整修复记录

---

### 在本仓验证
	cd ../../nccl-cc && make
	make test-arm              # 全量用例跑 ARM 后端 (qemu-arm)
	# 对比同一程序两个后端的输出
	./nccl-cc test/test_josephus.c | head -30
	./nccl-cc -target=arm test/test_josephus.c | head -30

### 本课文件
	../../nccl-cc/codegen_arm.c    — ARMv7 后端 (movw/movt, arm_imm_ok 旋转编码)
	../../nccl-cc/runtime_arm.s    — ARM Linux 平台层

> 课文源: [NCCL/Unit-4/Lesson-94.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-94.md) — 如有更新以书仓为准
