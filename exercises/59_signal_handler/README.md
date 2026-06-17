## Lesson 59 Signal Handler 信号处理与进程控制

### 代码

    /* 异步信号安全的处理函数 */
    static volatile sig_atomic_t got_sigint = 0;
    static void handle_sigint(int sig) {
        (void)sig;
        got_sigint = 1;  /* 只设标志, 不做复杂操作 */
    }

    /* 用 sigaction 安装处理函数 (比 signal 更可靠) */
    struct sigaction sa = { .sa_handler = handle_sigint, .sa_flags = SA_RESTART };
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);

    /* 看门狗: fork 子进程, 崩溃后自动重启 */
    while (restarts <= max_restarts) {
        pid_t pid = fork();
        if (pid == 0) { child_worker(); _exit(0); }
        waitpid(pid, &status, 0);
        if (WIFSIGNALED(status))
            printf("Child killed by signal %d, restarting...\n", WTERMSIG(status));
    }

### 知识点

- 信号 (Signal) 基础
  - 异步通知机制：内核→进程，进程→进程
  - `SIGINT` (Ctrl+C), `SIGTERM` (kill), `SIGKILL` (不可捕获)
  - `SIGCHLD`: 子进程退出时发给父进程
  - `SIGALRM`: `alarm()` 定时器到期
- 信号处理的正确姿势
  - 用 `sigaction` 而非 `signal` (语义更明确，跨平台一致)
  - handler 中只设 `volatile sig_atomic_t` 标志
  - 主循环检查标志并处理 — 避免在 handler 中做不安全操作
- 进程控制 `fork/waitpid/kill`
  - `fork()`: 复制进程，返回 0(子) 或 PID(父)
  - `waitpid()`: 父进程等待子进程退出
  - `WIFEXITED`/`WIFSIGNALED`: 检查退出方式
- 看门狗模式 (Watchdog)
  - 父进程监控子进程，异常退出则重启
  - 设最大重启次数防止死循环

### 课堂讨论

- 为什么 `SIGKILL` 不能被捕获？这有什么安全意义？
- `pause()` 和 `sleep()` 在信号处理上有什么区别？
- 在多线程程序中，信号被发送到哪个线程？

### 课后练习

- 用 Ctrl+C 测试：看门狗应优雅地终止子进程
- 添加 `SIGHUP` 处理：收到 HUP 信号时重新加载配置
- 挑战：实现一个简单的 `timeout` 命令 — 限时运行程序

### 参考资料

- APUE《UNIX 环境高级编程》第 10 章 信号
- signal-safety(7) 手册页 — 异步信号安全函数列表
