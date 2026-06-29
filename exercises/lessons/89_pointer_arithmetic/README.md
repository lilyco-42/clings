## Lesson 89 Pointer Arithmetic 指针运算

### 代码

    /* 输入 */
    int main() { int a[5]; int *p = a; *(p + 2) = 99; return a[2]; }    /* 期望 99 */

    /* p + 2 实际生成 p + 2*sizeof(int) */
        li a0, 2
        li t0, 4
        mul a0, a0, t0       # 2 → 8: parse 阶段插入的 ×4
        ...
        add a0, a0, t0       # p + 8

### 知识点

- 指针加法按"元素"而非"字节"
  - `p + n` ≡ 地址 + n × sizeof(\*p) — 缩放在 **parse 期**由 new_add() 注入 AST
  - 类型驱动：int* 缩放 4, char* 缩放 1, int\*\* 缩放 4 (ILP32 指针 4 字节)
  - 代码生成器只看见普通乘加 — 类型信息在 AST 构造时已消化完毕
- new_add 的三分支
  - int + int → 普通加法
  - ptr + int → int 先乘 sizeof — int + ptr 先交换再走同路
  - ptr + ptr → 编译错误 (无意义)
- ptr - ptr: 唯一合法的指针减法
  - `(q - p)` 生成 减法 + 除以 sizeof — 结果是"相隔几个元素"
  - 标准要求两指针同属一个数组 — 编译器不查，程序员负责
- Lesson 87 的回环
  - `a[2]` 脱糖成 `*(a+2)`, 走的正是本课的缩放路径
  - 数组与指针的"等价"只发生在表达式层 — 存储本质完全不同

### 课堂讨论

- `char *c = (char*)a; c + 2` 与 `a + 2` 差几个字节？强转为什么能改变缩放？
- p++ 对 int\* 是 +4 — 这个设计让数组遍历写成什么样？对比按字节加的世界
- LP64 平台 (x86-64/AArch64 后端) 指针 8 字节 — `int **pp; pp+1` 缩放多少？sizeof 谁说了算？

### 课后练习

- 测试：`*(p+4) - *(p+0)` 首尾元素差
- 测试：指针差 `return q - p;` (q = a+5, p = a) 应返回 5 而非 20
- 挑战：负下标 `p = a + 3; return p[-2];` — 缩放对负数也成立吗？(本编译器测试集 test_ptr_neg.c 的由来：AArch64 后端曾需 sxtw 符号扩展才答对)

### 参考资料

- N1256 6.5.6 — Additive operators (指针算术的标准语义)
- K&R 5.4 — Address Arithmetic

---

### 本课文件

    test_ptr_arith.c
    test_ptr_neg.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/89_pointer_arithmetic/test_ptr_arith.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-89.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-89.md) — 如有更新以书仓为准
