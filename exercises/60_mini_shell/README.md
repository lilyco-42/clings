## Lesson 60 Mini Shell 2.0 增强版迷你 Shell

### 代码

    /* 管道执行: cmd1 | cmd2 | cmd3 */
    for (int i = 0; i < nseg; i++) {
        if (i < nseg - 1) pipe(pipefd);
        pid_t pid = fork();
        if (pid == 0) {
            if (prev_fd != -1) dup2(prev_fd, STDIN_FILENO);
            if (pipefd[1] != -1) dup2(pipefd[1], STDOUT_FILENO);
            execvp(cmd->argv[0], cmd->argv);
        }
        prev_fd = pipefd[0]; /* 传递给下一阶段 */
    }

    /* 重定向: cmd < infile > outfile */
    if (cmd->infile) { dup2(open(cmd->infile, O_RDONLY), STDIN_FILENO); }
    if (cmd->outfile) { dup2(open(cmd->outfile, O_WRONLY|O_CREAT, 0644), STDOUT_FILENO); }

### 知识点

- 管道 (Pipe) 实现
  - `pipe()` 创建一对文件描述符：`pipefd[0]`(读) 和 `pipefd[1]`(写)
  - `dup2(pipefd[1], STDOUT)`: 子进程的标准输出连到管道写端
  - `dup2(prev_fd, STDIN)`: 子进程的标准输入连到上一级管道读端
  - 管道串联：每一阶段的 stdout → 下一阶段的 stdin
- 重定向 (Redirect)
  - `<`: 打开文件作为 stdin (`O_RDONLY`)
  - `>`: 打开文件作为 stdout (`O_WRONLY|O_CREAT|O_TRUNC`)
  - `>>`: 追加模式 (`O_APPEND`)
  - `dup2(fd, STDIN_FILENO)`: 将 fd 复制到标准输入
- `fork/exec` 模型
  - `fork`: 复制当前进程
  - `exec`: 在子进程中替换为新程序
  - 父进程 `waitpid` 等待所有子进程
- 内建命令 (Built-in)
  - `cd`: 必须在父进程中执行 (子进程 chdir 不影响父进程)
  - `pwd`, `exit`: 同理

### 课堂讨论

- 为什么 `cd` 必须是内建命令而不能用 `fork+exec`？
- 管道中的多个进程是并行还是串行执行？
- 如果管道中间的命令失败了，整条管道的退出码是什么？

### 课后练习

- 测试：`echo hello | tr a-z A-Z` 应输出 "HELLO"
- 添加 `&&` 和 `||` 逻辑连接符
- 添加后台执行 `&`: `sleep 10 &` 不等待子进程
- 挑战：添加命令历史 (readline 库) 和 Tab 补全

### 参考资料

- APUE《UNIX 环境高级编程》第 8,15 章 (进程控制，进程间通信)
- "Write a Shell in C" tutorial https://brennan.io/2015/01/16/write-a-shell-in-c/
