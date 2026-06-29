## Lesson 88 Pointers 指针

### 代码

    /* 输入 */
    int main() { int x = 10; int *p = &x; *p = 20; return x; }    /* 期望 20 */

    /* & 和 * 在汇编层的真面目 */
        addi a0, s0, -4      # &x — 就是 Lesson 77 的 gen_addr, 一条指令
        sw a0, -8(s0)        # p = &x (地址当普通数据存进 p 的槽)

        li a0, 20            # 右值 20
        ...
        addi a0, s0, -8      # &p
        lw a0, 0(a0)         # 读 p 的内容 = x 的地址
        ...
        sw t0, 0(a0)         # *(p) = 20 — 往 p 指向的地方写

### 知识点

- 指针 = 把地址当数据
  - `&x`: gen_addr 的结果不再立即 lw, 而是作为值存起来
  - `*p`: 先按普通变量读出 p 的值，再把它当地址去读/写一次
  - 全部 4 条核心指令在前面课程都出现过 — 指针没有新指令，只有新组合
- 左值/右值的递归统一
  - `*p = v`: 赋值左边是 ND_DEREF → gen_addr 就是"求 p 的值"
  - `**pp` 多级指针：解引用递归展开，实现里 1 行递归调用
- 指针类型链
  - Type 用 base 指针串起来：`int**` = ptr→ptr→int
  - 声明 `int **pp` 时每个 `*` 包一层 — parse 与类型构造一一对应
- 为什么 `int *p` 的 `*` 贴变量名
  - C 声明哲学："声明形式 = 使用形式" — `*p` 是 int, 所以 p 是 int\*

### 课堂讨论

- `int *p; *p = 1;` — p 没初始化指向哪里？野指针在本课的输出码里是哪条指令爆炸？
- swap(int *a, int *b) 为什么必须传指针？画出 main 帧与 swap 帧，标出 a 指向哪
- `&(&x)` 为什么不合法？gen_addr 对 ND_ADDR 节点该怎么报错？

### 课后练习

- 测试：经典 swap — `swap(&x, &y)` 后 x、y 互换
- 测试：`int **pp = &p; **pp = 7;` 二级指针链
- 用 `-dump-ast` 观察 `*p = 20` 的树：ASSIGN 左孩子是 DEREF

### 参考资料

- K&R 5.1-5.2 — Pointers and Addresses
- chibicc step "Add unary & and \*"

---

### 本课文件

    test_pointer.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/88_pointers/test_pointer.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-88.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-88.md) — 如有更新以书仓为准
