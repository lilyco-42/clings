# nccl-cc — 新概念 C 语言编译器

一个为教学而生的 C 子集编译器：**一棵 AST，四个后端，五个运行环境**。
用 ~4000 行 C 代码完整走通 预处理 → 词法 → 语法 → 类型标注 → 代码生成 的全流程。

```
            ┌──────────┐   ┌──────────┐   ┌─────────┐   ┌──────────┐
 source.c → │preprocess│ → │ tokenize │ → │  parse  │ → │ add_type │
            └──────────┘   └──────────┘   └─────────┘   └────┬─────┘
                                                             │ 带类型的 AST
               ┌─────────────┬─────────────┬─────────────┬───────┘
               ▼              ▼              ▼              ▼
         codegen_rv.c   codegen_arm.c   codegen_a64.c  codegen_x86.c
          RISC-V 32      ARMv7 32       AArch64(LP64)   x86-64(LP64)
               │
               ├── Linux 用户态（qemu-riscv32）
               └── 裸机（qemu-system-riscv32 virt：UART + test finisher）
```

## 平台支持矩阵

| 后端                    | 数据模型         | 汇编                                              | 链接                                         | 运行                                           |
| ----------------------- | ---------------- | ------------------------------------------------- | -------------------------------------------- | ---------------------------------------------- |
| `rv32`（默认）          | ILP32（指针 4B） | `riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32` | `-nostdlib + runtime_rv_io.s + runtime_rv.s` | `qemu-riscv32`                                 |
| `rv32` 裸机             | ILP32            | 同上 + `-T bare_rv.ld -Wl,--build-id=none`        | `runtime_rv_io.s + runtime_rv_bare.s`        | `qemu-system-riscv32 -machine virt -bios none` |
| `arm`                   | ILP32（指针 4B） | `arm-linux-gnueabihf-gcc -marm`                   | `-nostdlib + runtime_arm.s`                  | `qemu-arm`                                     |
| `a64`                   | LP64（指针 8B）  | `aarch64-linux-gnu-gcc`                           | `-nostdlib + runtime_a64.s`                  | `qemu-aarch64`                                 |
| `x64` / `x86`           | LP64（指针 8B）  | 宿主 `cc`                                         | libc（真 printf）                            | 原生                                           |
| `x86-mac` / `x86-linux` | LP64             | 交叉发射 Mach-O / ELF 语法                        | —                                            | —                                              |

裸机路径是同一份编译产物换了平台层：`putchar` 变成写 16550A UART
寄存器（0x10000000），`exit` 变成写 SiFive test finisher（0x100000）请求
模拟器退出——退出码照常传递，`// expect: N` 测试头零改动复用。

**macOS 用户注意**：

- Intel Mac：`cc out.s` 直接可用（macOS 26 Tahoe 是最后支持 Intel 的版本）。
- Apple Silicon：`cc -arch x86_64 out.s` 经 Rosetta 2 透明运行——该通道在 macOS 27 之前可用，
  之后 Rosetta 仅保留游戏兼容。AArch64 后端（`-target=a64`）已在 Linux ELF +
  qemu 上全量验证；原生 Mach-O arm64 还需 Apple 变参 ABI（变参入栈）与
  `@PAGE/@PAGEOFF` 重定位语法，列入后续计划。
- 自带 runtime（`runtime_*.s`）使用 Linux 系统调用，**仅限 Linux**；macOS 一律走 libc。

## 快速开始

```sh
make                                  # 构建编译器（任意 gcc/clang 即可）

./nccl-cc test/test_josephus.c > j.s  # RISC-V 汇编（默认目标）
riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static \
    j.s runtime_rv_io.s runtime_rv.s -o j
qemu-riscv32 ./j; echo $?             # => 30

./nccl-cc -target=x64 test/test_josephus.c > j.s
cc j.s -o j && ./j; echo $?           # => 30（本机直跑）

# 裸机：同一份 j.s，换裸机平台层 + 链接脚本，跑在没有 OS 的虚拟板子上
./nccl-cc test/test_josephus.c > j.s
riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static \
    -Wl,--build-id=none -T bare_rv.ld j.s runtime_rv_io.s runtime_rv_bare.s -o j.elf
qemu-system-riscv32 -machine virt -nographic -bios none -kernel j.elf; echo $?  # => 30
```

## 测试

```sh
make test        # RISC-V 后端（qemu 用户态）
make test-arm    # ARM 后端（qemu）
make test-a64    # AArch64 后端（qemu）
make test-x86    # x86-64 后端（原生）
make test-bare   # RISC-V 裸机（qemu-system，无 OS）
make test-all    # 五路全量
make fuzz        # 差分模糊：随机程序 × 四后端 × gcc 参照（FUZZ_N=50）
```

- `test/*.c` 首行以 `// expect: N` 声明期望退出码，runner 逐一比对；
- `test/invalid/*.c` 是必须被编译器**拒绝**的坏程序（退出码非零且不挂死，
  编译 10s/运行 20s 超时上限，挂起会被明确报为 HUNG）；
- 任一失败 `make` 返回非零 → CI 真实变红；
- 所有期望值都经过宿主 `gcc` 差分校验（同一程序、同一退出码）；
- `make fuzz` 用生成器（`fuzz/gen_prog.c`，种子可复现、UB 按构造排除）
  随机产生合法程序——标量算术/控制流/函数调用之外还覆盖数组读写与
  指针 deref/重指（下标恒 `&15`、指针只在所属数组界内游走）——逐个与
  `gcc -fwrapv` 参照比对四后端退出码；失配样本存 `fuzz/failures/seed_N.c`。
  首轮即抓出 ARM 字面量池溢出 bug。

## 命令行

```
nccl-cc [options] <source.c>
  -target=rv32|arm|a64|x64    选择目标（x86 为 x64 别名，aarch64 为 a64 别名）
  -target=x86-mac|x86-linux   x86-64 并强制 Mach-O / ELF 语法（交叉发射）
  -o FILE                     输出到文件
  -E                          仅预处理
  -D NAME[=VALUE]             预定义宏
  -dump-tokens                打印 token 流后退出
  -dump-ast                   打印带类型标注的 AST 后退出
```

`-dump-ast` 会显示每个表达式的类型与局部变量栈偏移，是排查问题的第一抓手：

```
ASSIGN  :: pointer
  VAR 'p'  :: pointer  [fp-8]
  ADDR  :: pointer
    VAR 'x'  :: int  [fp-4]
```

## 支持的 C 子集

int / char / 指针（含多级）/ 一维数组 / struct（成员限 int、char 及其指针）/
enum（编译期常量）/ 全部算术·位·逻辑·比较运算 / 复合赋值 / 前后缀 `++ --` /
三元 / 逗号 / `(type)` 强转 / sizeof / if-else / while / for / do-while /
switch-case / break / continue / 函数定义与调用 / 全局变量 / 字符串字面量 /
`#define` `#include "..."` `#ifdef/#ifndef/#else/#endif` `#undef`

## 已知限制（教学取舍，按设计接受）

- **无函数原型**：所有函数按"返回 int"处理；参数个数不做跨函数检查
  （上限：RV 8 个 / ARM 4 个 / AArch64 8 个 / x86-64 6 个，超限 a64/x86 后端显式报错）。
- **无块级作用域**：同一函数内同名变量共享一个槽位。
- **RV/ARM 表达式栈未保持 16/8 字节对齐**：自带 runtime 不受影响，
  但与真实 libc 链接时违反 psABI（x86-64 与 AArch64 后端是严格对齐的：
  前者靠 depth 计数，后者是硬件强制 16 字节压栈，三种策略对比是一课）。
- **struct**：不支持嵌套 struct 成员、struct 传参/返回、struct 赋值。
- **预处理器**：宏不支持参数；`#if` 表达式不支持（只有 ifdef/ifndef）。
- 字符串字面量中的内嵌 `\0` 会截断（按 C 字符串处理）。

## 文件导览

| 文件                                   | 职责                                                                |
| -------------------------------------- | ------------------------------------------------------------------- |
| `nccl_cc.h`                            | Token / AST / Type / Var 数据结构与全局声明                         |
| `preprocess.c`                         | 文本级预处理：宏、include、条件编译                                 |
| `tokenize.c`                           | 词法：转义在此**一次性解码**；`emit_escaped_string` 在发射时再编码  |
| `parse.c`                              | 递归下降；指针运算缩放统一走 `new_add/new_sub`                      |
| `type.c`                               | 类型构造、`target_ptr_size`（ILP32/LP64）、`add_type` 标注 pass     |
| `codegen_rv.c`                         | RISC-V 后端（帧布局：ra/s0 存帧底，详见文件头注释）                 |
| `codegen_arm.c`                        | ARM 后端（armv7ve；movw/movt 材料化常量，不用字面量池）             |
| `codegen_a64.c`                        | AArch64 后端（LP64；w/x 宽度选择、硬件 16B 栈对齐、sxtw 指针缩放）  |
| `codegen_x86.c`                        | x86-64 后端（宽度感知 load/store、SysV 16B 调用对齐、%al 变参约定） |
| `runtime_rv_io.s`                      | RV 平台无关层：print_int/printf（只依赖 putchar 契约）              |
| `runtime_rv.s` / `runtime_rv_bare.s`   | RV 平台层：Linux syscall vs 裸机 UART/finisher                      |
| `bare_rv.ld`                           | 裸机链接脚本：\_start 钉死 0x80000000，丢弃 note 段                 |
| `runtime_arm.s` / `runtime_a64.s`      | ARM / AArch64 runtime（Linux syscall）                              |
| `debug.c`                              | `-dump-tokens` / `-dump-ast`                                        |
| `test/run_tests.sh`                    | 五路统一测试 runner（后端定义共享自 `test/backends.sh`）            |
| `fuzz/gen_prog.c` + `fuzz/run_fuzz.sh` | 差分模糊：随机程序 × 四后端 × gcc 参照                              |
