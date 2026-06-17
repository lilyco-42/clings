## Lesson 91 Struct Support 结构体

### 代码

    /* 输入 */
    struct Point { int x; int y; };
    int main() {
        struct Point p;
        p.x = 3; p.y = 4;
        return p.x + p.y;
    }

    /* p.y 的寻址: 结构体基址 + 成员偏移 */
        addi a0, s0, -8      # &p (结构体整体占 8 字节)
        addi a0, a0, 4       # + y 的偏移 4 = &p.y
        ...                  # 之后与普通 int 读写无异

### 知识点

- struct = 带名字的偏移表
  - parse 期逐成员累加偏移：x→0, y→4 — Member 链表挂在 Type 上
  - `p.x` 生成 gen_addr(p) + `addi a0, a0, offset` — 一条加法而已
  - struct 整体大小进 stack_size, 一次分配
- 对齐 (alignment) 的最简策略
  - 成员按自身大小对齐：char 后跟 int 要补 3 字节空洞
  - `struct { char c; int x; }` 大小是 8 不是 5 — sizeof 必须算上 padding
- 成员当左值
  - `p.x = 3`: ND_MEMBER 进 gen_addr 的第三个分支 (VAR/DEREF 之后)
  - 地址链可以延长：`pp->x` 在支持 `->` 后就是 DEREF + MEMBER 的组合
- 教学边界 (按设计取舍)
  - 成员限 int/char 及其指针; 不支持嵌套 struct、struct 传参/返回/整体赋值
  - 这些"不支持"逼出清晰的错误信息 — 比默默生成错码好得多

### 课堂讨论

- 为什么需要 padding？RISC-V 的 lw 对非 4 对齐地址会怎样？(对比 x86 的宽容)
- `struct Point p2 = p1;` 整体赋值要生成什么代码？memcpy 还是逐字段？
- 把 y 的偏移算错 1 字节会发生什么？— 偏移表错误是二进制兼容性问题的根源 (ABI)

### 课后练习

- 测试：`struct { char a; char b; int c; }` — sizeof 应为 8, 验证你的 padding 理解
- 测试：结构体指针成员 `struct Node { int val; struct Node *next; };` 能否声明？
- 挑战：实现 `->` 运算符 — 提示：`p->x` ≡ `(*p).x`, 又是一次脱糖

### 参考资料

- N1256 6.7.2.1 — Structure layout 与实现自由度
- 《深入理解计算机系统》(CS:APP) 3.9 — 异质数据结构

---

### 本课文件

    test_struct.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/91_struct_support/test_struct.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-91.md](https://cnb.cool/q.qq/opencamp-c-2026-summer/NCCL/-/blob/master/Unit-4/Lesson-91.md) — 如有更新以书仓为准
