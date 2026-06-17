## Lesson 73 Return a Constant 返回常量编译器

### 代码

    /* 输入 */
    int main() { return 42; }

    /* 输出 (RISC-V 汇编) */
    .globl main
    main:
        li a0, 42
        ret

### 编译器管线

    源代码 → tokenize() → parse() → codegen_rv() → RISC-V 汇编

### 知识点

- 编译器最小骨架
  - main.c: 读取源文件，调用各阶段，输出到 stdout
  - tokenize.c: 识别 int, main, return, 数字，括号，分号
  - parse.c: 解析 program → function → return stmt → number
  - codegen_rv.c: 生成 `.globl main` / `li a0, N` / `ret`
- 从 Lesson 71 解释器到编译器的转变
  - 解释器：parse → eval() → 返回整数值
  - 编译器：parse → codegen() → 输出汇编文本
  - AST 结构不变，只换后端
- 验证方法
  - 编译器输出 .s 文件 → GNU as 汇编 → 链接 → QEMU 运行 → 检查退出码

### 编译运行

    gcc -o nccl-cc main.c tokenize.c parse.c codegen_rv.c
    ./nccl-cc test_return.c > test.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static test.s -o test
    qemu-riscv32 ./test
    echo $?   # 应输出 42

### 课堂讨论

- 这个编译器和 Lesson 71 的解释器共享了多少代码？
- 为什么输出汇编文本而不是直接生成二进制？
- `li a0, 42` 是伪指令，汇编器会把它变成什么？

### 课后练习

- 修改编译器使其支持负数返回值：`return -1;`
- 支持十六进制字面量：`return 0xFF;`
- 对比编译器输出与 `riscv64-linux-gnu-gcc -S` 的输出差异

### 参考资料

- chibicc 第 1 个 commit https://github.com/rui314/chibicc/commit/0522e2d
