## Lesson 95 Cross-Compilation & Testing Engineering 交叉编译与测试工程

### 代码

    # 一条命令跑全部后端 × 全部用例
    make test        # RISC-V (qemu 用户态)
    make test-arm    # ARM
    make test-a64    # AArch64
    make test-x86    # x86-64 (原生)
    make test-bare   # RISC-V 裸机 (qemu-system, 无操作系统!)
    make test-all    # 五路全量
    make fuzz        # 差分模糊: 随机程序 × 四后端 × gcc 参照

    # 每个测试用首行声明自己的期望
    // expect: 30
    int main() { ... }

    # runner 核心循环 (test/run_tests.sh)
    expect=$(sed -n 's|^// expect: ||p' "$t")
    timeout 10 ./nccl-cc $TARGET_FLAG "$t" > "$OUT.s"   # 编译 (限时!)
    assemble "$OUT.s" "$OUT.bin"                         # 交叉工具链
    timeout 20 $RUN "$OUT.bin" </dev/null >/dev/null 2>&1
    got=$?                                               # 退出码即结果

### 知识点

- 期望值驱动测试：// expect 头
  - 用例自描述，runner 零配置; 新增测试 = 添一个 .c 文件
  - 所有期望值经宿主 gcc 差分校验 — 期望不是人拍的，是 gcc 算的
- 工具链矩阵 (交叉编译的全景)
  - 编译器 (本机) → 汇编器 (riscv64-linux-gnu-gcc 等交叉工具) → 模拟器 (qemu)
  - backends.sh 把"每个后端怎么汇编/怎么运行"收敛成一张表，runner 与 fuzzer 共用
- 负面测试与超时守卫
  - test/invalid/\*.c 必须被**拒绝** (退出码非零) — 错误处理也是功能
  - 每步 timeout: 死循环的 parser 报 HUNG 而非挂死 CI — "挂起 ≠ 拒绝"
- 差分模糊：让随机程序当测试员
  - 生成器按种子产出合法 C 程序 (UB 按构造排除：除数恒非零、移位掩码、下标 &15)
  - gcc -fwrapv 编译做参照，四后端逐个比退出码 — 任何不一致都是 bug
  - 战果：第一轮 50 个种子就抓出 ARM 字面量池溢出 (Lesson 94) — 手写用例永远写不出那么长的函数
  - 大栈帧立即数编码 bug (RV ±2047 / ARM 旋转编码) 也是同类"用例够怪才触发"的问题

### 课堂讨论

- 为什么期望退出码而不比对汇编文本？汇编比对会"锁死"什么？
- 模糊测试生成的程序必须**无 UB** — 如果生成 `1/0` 或有符号溢出，差分还公平吗？
- CI 里 qemu-system 启动一次 ~0.5 秒 × 39 用例 — 裸机测试为何单列不进默认门禁？

### 课后练习

- 给 runner 添一个故意错误的期望值，观察 CI 如何"真实变红"
- 跑 `make fuzz FUZZ_N=200 FUZZ_SEED=你的学号` — 你的种子区间能抓到新 bug 吗？
- 阅读 fuzz/gen_prog.c 的 UB 排除注释 — 每条规则对应哪种未定义行为？

### 参考资料

- McKeeman, "Differential Testing for Software" (1998)
- Csmith: 随机 C 程序生成器找编译器 bug 的工业级先例

---

### 在本仓验证

    cd ../../nccl-cc && make
    make test-all              # 五路全量: rv / arm / a64 / x86 / rv-bare
    make fuzz FUZZ_N=50        # 差分模糊: 随机程序 x 四后端 x gcc 参照

### 本课文件

    ../../nccl-cc/test/run_tests.sh — 五路统一 runner (// expect 头驱动)
    ../../nccl-cc/test/backends.sh  — 后端矩阵: 汇编/运行方式一张表
    ../../nccl-cc/test/invalid/     — 必须被拒绝的负面用例
    ../../nccl-cc/fuzz/gen_prog.c   — 随机程序生成器 (UB 按构造排除)

> 课文源：[NCCL/Unit-4/Lesson-95.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-95.md) — 如有更新以书仓为准
