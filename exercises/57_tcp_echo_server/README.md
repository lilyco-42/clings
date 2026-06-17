## Lesson 57 Echo Server TCP 回声服务器

### 代码

    /* socket编程五步曲 */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);  /* 1. 创建 */
    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));  /* 2. 绑定 */
    listen(server_fd, BACKLOG);                        /* 3. 监听 */
    while (1) {
        int client_fd = accept(server_fd, ...);        /* 4. 接受 */
        handle_client(client_fd);                      /* 5. 处理 */
    }

    /* 回声处理: 读什么就写什么 */
    static void handle_client(int client_fd, ...) {
        char buf[4096];
        ssize_t n;
        while ((n = read(client_fd, buf, 4096)) > 0) {
            write(client_fd, buf, n);  /* echo back */
        }
        close(client_fd);
    }

### 知识点

- Socket 编程基本流程
  - `socket()` → `bind()` → `listen()` → `accept()` → `read/write` → `close()`
  - `AF_INET` + `SOCK_STREAM` = TCP/IPv4
  - `htons()` / `ntohs()`: 主机字节序 ↔ 网络字节序
- 地址结构 `struct sockaddr_in`
  - `.sin_family = AF_INET`
  - `.sin_port = htons(port)` (网络字节序)
  - `.sin_addr.s_addr = INADDR_ANY` (监听所有接口)
- 可靠写入 (write loop)
  - `write` 可能只写入部分数据 → 循环直到全部写完
  - 这是网络编程的常见陷阱
- `SO_REUSEADDR`
  - 避免服务器重启时 "Address already in use" 错误
  - TIME_WAIT 状态下的端口可以立即复用

### 课堂讨论

- 当前实现是串行的：一次只能服务一个客户端，如何改进？
- `read` 返回 0 表示什么？返回 -1 呢？
- TCP 和 UDP echo server 有什么区别？

### 课后练习

- 用 `nc` (netcat) 测试：`echo "hello" | nc localhost 9000`
- 改为 fork 模式：每个客户端 fork 一个子进程处理
- 挑战：用 `select()` 或 `poll()` 实现多客户端并发

### 参考资料

- Beej's Guide to Network Programming https://beej.us/guide/bgnet/
- Stevens《UNIX 网络编程》卷 1 第 5 章
