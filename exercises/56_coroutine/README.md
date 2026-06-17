## Lesson 56 Coroutine 协程实现

### 代码

    /* co_yield: 保存当前上下文, 跳回调度器 */
    void co_yield(void)
    {
        if (setjmp(coroutines[current].ctx) == 0) {
            coroutines[current].state = CO_SUSPENDED;
            longjmp(scheduler_ctx, 1);  /* 回到调度器 */
        }
        /* longjmp 回来时从这里恢复执行 */
    }

    /* 轮询调度器: 依次恢复每个协程 */
    void co_schedule(void)
    {
        while (1) {
            int all_dead = 1;
            for (int i = 0; i < co_count; i++) {
                if (coroutines[i].state == CO_SUSPENDED)
                    co_resume(i);
            }
            if (all_dead) break;
        }
    }

### 知识点

- setjmp/longjmp 的本质
  - `setjmp`: 保存当前 CPU 寄存器快照 (SP, PC, callee-saved)
  - `longjmp`: 恢复寄存器快照 → 跳回 `setjmp` 返回点
  - **这就是上下文切换的最小实现**
- 协程 vs 线程
  - 协程：**协作式**调度，主动 `co_yield` 让出 CPU
  - 线程：**抢占式**调度，OS 定时器强制切换
  - 协程不需要锁：同一时刻只有一个在运行
- 栈帧布局 (编译器关联)
  - 每个协程有独立栈空间 (32KB)
  - `co_trampoline` 在新栈上启动函数
  - **理解栈帧是编译器代码生成的核心** — 函数调用如何分配/释放栈帧
- 生产者 - 消费者模式
  - 无锁通信：通过共享变量 + `co_yield` 协作

### 课堂讨论

- `setjmp` 保存了哪些寄存器？为什么不需要保存 caller-saved 寄存器？
- 如果协程函数中调用 `alloca` 或 VLA, 会有什么问题？
- Python 的 `yield` 和本课的 `co_yield` 有什么异同？

### 课后练习

- 给协程添加"返回值": `co_yield_value(int val)` + `co_recv()`
- 实现 3 个协程互相切换 (A→B→C→A→...)
- 挑战：用 `ucontext.h` 的 `makecontext`/`swapcontext` 替代 setjmp 实现

### 参考资料

- POSIX `setjmp(3)` / `longjmp(3)` 手册
- Simon Tatham, "Coroutines in C" https://www.chiark.greenend.org.uk/~sgtatham/coroutines.html
