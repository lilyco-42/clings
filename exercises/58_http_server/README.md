## Lesson 58 Mini HTTP Server 迷你 HTTP 服务器

### 代码
	/* 解析 HTTP 请求行: "GET /path HTTP/1.0" */
	static int parse_request(const char *req, char *method, char *path)
	{
	    return sscanf(req, "%15s %255s", method, path) == 2 ? 0 : -1;
	}
	
	/* 发送文件: HTTP头 + 文件内容 */
	static void send_file(int fd, const char *path)
	{
	    FILE *fp = fopen(fullpath, "rb");
	    if (!fp) { send_error(fd, 404, "Not Found"); return; }
	    /* 发送 "HTTP/1.0 200 OK\r\nContent-Type: ...\r\n\r\n" */
	    /* 然后用 fread+write 循环发送文件内容 */
	}

### 知识点
* HTTP/1.0 协议要素
	- 请求行: `METHOD /path HTTP/1.0\r\n`
	- 头部: `Key: Value\r\n` (以空行 `\r\n` 结束)
	- 本实现仅支持 GET (最简有效)
* Content-Type 检测
	- 根据文件扩展名查表: `.html`→`text/html`, `.js`→`application/javascript`
	- 默认: `application/octet-stream`
* 安全防护
	- 拒绝路径中包含 `..` 的请求 (目录遍历攻击)
	- 目录请求自动重定向到 `index.html`
* 与编译器的联系
	- HTTP 请求解析 = 文本协议的词法+语法分析
	- `parse_request` 用 `sscanf` — 编译器用更精细的 tokenizer

### 课堂讨论
* HTTP/1.0 和 HTTP/1.1 的主要区别是什么？(Keep-Alive)
* 为什么 `Content-Length` 头部很重要？
* 如何防止目录遍历攻击 (path traversal) 以外的安全问题？

### 课后练习
* 用浏览器访问 `http://localhost:8080/index.html`
* 添加 HEAD 方法支持 (只返回头部, 不返回 body)
* 挑战: 添加 POST 方法 + CGI 脚本执行 (将 POST body 传给外部程序)

### 参考资料
* HTTP/1.0 规范 RFC 1945
* "Tiny Web Server" (CSAPP 教材配套实验)
