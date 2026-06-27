## Lesson 47 — setjmp/longjmp 非局部跳转

### 课程任务

使用 `setjmp/longjmp` 实现跨函数栈的非局部跳转：

1. 在 `main()` 中用 `setjmp(env)` 设锚点（保存执行环境）
2. 在深层嵌套函数 `funcC()` 中检测到错误时，用 `longjmp(env, 1)` 直接"空降"回 main
3. 调用链: `main → funcA → funcB → funcC → longjmp → main`（跳过中间所有栈帧）

```
验证用例:
  输入 "error\n" → caught error: error
  输入 "ok\n"    → funcC: ok → funcB done → funcA done → success: ok
```

### 前置知识：什么是"非局部跳转"？

C 语言中，`return` 只能跳回上一层调用者。`goto` 只能跳到同函数内的标签。`setjmp/longjmp` 是 C 语言中唯一能**跨越多层函数栈帧**的跳转机制：

```
正常 return 路径:                 longjmp 路径:
  main                              main
   → funcA                           → funcA
     → funcB                           → funcB
       → funcC                           → funcC
         → return                         → longjmp ──────┐
       ← return                            (跳过所有)      │
     ← return                                          ←──┘ main
   ← return                                       (直达 main 的 setjmp 位置!)
```

### setjmp 和 longjmp 的签名

```c
#include <setjmp.h>

jmp_buf env;                          // 保存执行环境的缓冲区

int setjmp(jmp_buf env);              // 设锚点: 保存当前环境, 返回 0
void longjmp(jmp_buf env, int val);   // 跳回: 恢复环境, 让 setjmp 返回 val
```

| 函数 | 调用位置 | 效果 |
|------|---------|------|
| `setjmp(env)` | 在 main 中（你想"回来"的地方） | 保存 SP/PC/寄存器到 env，返回 0 |
| `longjmp(env, val)` | 在深层函数中（错误发生时） | 恢复 env 中保存的寄存器，让 setjmp 返回 val |

### 返回值语义（核心考点）

`setjmp` 返回**两次**——一次是初次调用，一次是 longjmp 跳回：

```
第一次: setjmp(env) → 返回 0  (正常初始化路径)
第二次: longjmp(env, 1) → setjmp(env) 返回 1 (跳回路径)

通过检查返回值区分两种路径:
  if (r == 0) { /* 正常路径 */ }
  else       { /* 异常/跳回路径 */ }
```

### CPU 寄存器视角（底层原理）

```
setjmp(env) 保存什么？
  ┌─────────────────┐
  │ SP  (栈指针)     │ → 保存当前栈帧位置
  │ PC  (程序计数器)  │ → 保存当前执行位置
  │ BP  (基址指针)   │ → 保存当前栈帧基址
  │ 通用寄存器       │ → 保存 callee-saved 寄存器
  └─────────────────┘

longjmp(env, val):
  1. 从 env 恢复 SP → 栈被"切回"到 setjmp 时的位置
  2. 从 env 恢复 PC → 执行流跳回 setjmp 处
  3. 从 env 恢复其他寄存器 → 上下文完全恢复
  4. setjmp 这次返回 val（而非 0）
```

### 完整执行流程图解

#### 正常路径（输入 "ok"）

```
main:
  setjmp(env) → 返回 0
  r == 0 → funcA("ok")
    funcA → funcB("ok")
      funcB → funcC("ok")
        funcC: strcmp("ok","error")!=0 → printf("funcC: ok")
      funcB: printf("funcB done")
    funcA: printf("funcA done")
  main: printf("success: ok")

输出: funcC: ok
      funcB done
      funcA done
      success: ok ✓
```

#### 跳回路径（输入 "error"）

```
main:
  setjmp(env) → 保存环境 → 返回 0
  r == 0 → funcA("error")
    funcA → funcB("error")
      funcB → funcC("error")
        funcC: strcmp("error","error")==0
        → longjmp(env, 1) ──────────────────┐
        (funcC 的后续代码不执行!)              │
      funcB 的 printf 不执行!                  │
    funcA 的 printf 不执行!                    │
  main: setjmp 这次返回 1 ←───────────────────┘
  r == 1 → printf("caught error: error")

输出: caught error: error ✓
```

### 被跳过栈帧的局部变量

```
void funcC() {
    int x = 42;        // ← 这个局部变量存储在被跳过的栈帧里
    longjmp(env, 1);   // 跳走!
    // x 的析构永远不会执行（C 中没有析构，但在 C++ 中是大问题）
}
```

**C 中**：局部变量随栈帧被"丢弃"，不会有额外副作用。
**C++ 中**：被跳过栈帧中对象的析构函数不会被调用！因此 C++ 标准不建议在可能包含非平凡析构对象的函数中使用 longjmp。这就是 C++ 需要 `try/catch` 的原因之一。

### try-catch 与 setjmp/longjmp 的关系

```
C++ 异常处理（简化原理）:

try {
    /* = setjmp(env) */
    do_work();
} catch (std::exception& e) {
    /* = setjmp 返回非 0 */
}

throw e;  /* = longjmp(env, 1) */

实际上现代 C++ ABI (Itanium C++ ABI) 使用更复杂的展开机制，
但核心思想确实来源于 setjmp/longjmp。
```

### 常见错误

| 错误 | 后果 | 正确 |
|------|------|---------|
| longjmp 的 val 传 0 | setjmp 返回 0，无法区分跳回 | val 必须非 0 |
| setjmp 赋给非 volatile 变量 | 编译器可能优化掉检查 | `volatile int r = setjmp(env)` |
| jmp_buf 是局部变量 | longjmp 时 env 已失效 | jmp_buf 必须是全局或静态变量 |
| longjmp 跳到已返回的函数 | 未定义行为 | 确保 setjmp 的栈帧仍在 |
| C++ 中跳过多层构造函数 | 析构函数泄漏 | C++ 中优先用 try/catch |

### 重要知识点

- `setjmp` 首次返回 0，`longjmp` 跳回后返回 val（非 0）
- `longjmp` 直接操作 SP/PC 寄存器——不是魔法，是压缩的上下文保存/恢复
- C++ 的 try/catch 底层机制起源于 setjmp/longjmp
- 被跳过栈帧的局部变量不会被清理——C++ 中这是大问题

### 课堂讨论

1. **setjmp/longjmp 和 goto 有什么本质区别？** — goto 只能跳到同函数内的标签，受编译器作用域约束。longjmp 跳转是由运行时决定的——可以从任意深度的函数跳回 setjmp 位置。goto 是编译期确定的，longjmp 是运行期确定的。

2. **C++ 的 try/catch 底层如何用 setjmp/longjmp 实现？** — Itanium C++ ABI 使用两级展开：Phase 1 搜索 catch 处理器（类似 setjmp），Phase 2 实际展开栈（类似 longjmp + 析构函数调用）。现代实现已经远超简单 setjmp/longjmp，但核心思想相通。

3. **longjmp(env, 0) 会怎样？** — setjmp 返回 0，和初始调用无法区分——你将不知道是"刚设锚点"还是"从 longjmp 跳回"。标准要求 val 应为非 0 值。

4. **setjmp/longjmp 实际的工程用途是什么？** — 嵌入式系统的错误恢复、协程库的底层实现、Lua 等脚本语言的异常处理。现代应用开发中较少直接使用，多被 try/catch 替代。

5. **为什么不能用局部 jmp_buf？** — setjmp 只保存寄存器上下文，不拷贝栈。当函数返回后，longjmp 尝试恢复已失效栈帧的寄存器值，导致未定义行为（通常是段错误或跳转到随机地址）。

### 后续衔接

- Lesson 46: sscanf —— 解析出错误输入时如何优雅处理
- Lesson 48: I/O 性能 —— 文件操作的底层开销
- C++ 对比: try/catch/throw —— 现代异常处理
- 高阶: 实现一个简单的协程库（用 setjmp/longjmp 切换上下文）

### 寄存器保存的精确内容

```
x86-64 上 setjmp 保存的具体寄存器 (glibc 实现):

┌──────────────────────────────────────────┐
│ JB_RBX  (0x00): %rbx — 基址寄存器        │
│ JB_RBP  (0x08): %rbp — 栈帧基址指针      │
│ JB_R12  (0x10): %r12                     │
│ JB_R13  (0x18): %r13                     │
│ JB_R14  (0x20): %r14                     │
│ JB_R15  (0x28): %r15                     │
│ JB_RSP  (0x30): %rsp — 栈指针 ← 关键!    │
│ JB_PC   (0x38): %rip — 程序计数器 ← 关键! │
│ 信号掩码 (可选的 sigsetjmp)               │
└──────────────────────────────────────────┘

longjmp 操作:
  mov (%env), %rbx     → 恢复所有 callee-saved 寄存器
  mov 0x30(%env), %rsp → 切回 setjmp 时的栈位置
  jmp *0x38(%env)      → 跳回 setjmp 的下一条指令
```

### 工业级应用实例

```
Lua 协程 (lstate.c):
  lua_yield → longjmp 回到调度器
  lua_resume → setjmp 锚点后执行

libsigsegv (页错误处理):
  sigsetjmp 在栈上设锚点
  SIGSEGV 处理器调用 siglongjmp 跳回

PostgreSQL 异常处理:
  PG_TRY / PG_CATCH 宏底层使用 sigsetjmp/siglongjmp
```

### C 标准对 setjmp 使用的限制

```
C 标准 §7.13.1.1 明确规定 setjmp 只能出现在:
  1. 选择语句 (if, switch) 的控制表达式
  2. 迭代语句 (while, for) 的控制表达式
  3. 比较运算符 (==, !=, <, ...) 的一个操作数
  4. 一元 ! 运算符的操作数
  5. 作为整个表达式语句 (expr;)

违反则行为未定义!

合法: if (setjmp(env) == 0) { ... }
      switch (setjmp(env)) { ... }
      r = setjmp(env);

非法: foo(setjmp(env));     // 函数参数
      x = setjmp(env) + 1;  // 复杂表达式
```

### 参考资料

- `man setjmp` — 标准库文档
- C 标准 §7.13 — Non-local jumps
- C++ 标准 §18.10 — 与 C 的兼容性说明
- glibc `setjmp/longjmp` 源码 — `sysdeps/x86_64/setjmp.S`
