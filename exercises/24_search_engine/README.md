## Lesson 24 Build a Simple Search Engine 简单搜索引擎

### 代码

    /* 从 HTML 中提取所有 href="..." 链接 */
    static int extract_links(const char *html)
    {
        const char *p = html;
        int count = 0;
        while ((p = find_istr(p, "href=")) != NULL) {
            p += 5;
            char quote = (*p == '"' || *p == '\'') ? *p++ : 0;
            char url[1024];
            int i = 0;
            while (*p && i < 1023) {
                if (quote && *p == quote) break;
                if (!quote && (*p == ' ' || *p == '>')) break;
                url[i++] = *p++;
            }
            url[i] = '\0';
            if (i > 0) printf("[%d] %s\n", ++count, url);
        }
        return count;
    }

### 知识点

- 字符串匹配与指针操作
  - `find_istr`: 大小写不敏感的子串查找 (手写实现)
  - 指针递进：`p += 5` 跳过 "href=", 然后逐字符提取 URL
- HTML 解析的简化处理
  - 只关注 `href="..."` 属性，不解析完整 DOM
  - 处理单引号和双引号两种包围方式
- 文件 I/O 与动态内存
  - `read_all()`: 用 `fread` + `realloc` 将整个文件读入内存
  - 支持 stdin 管道输入：`curl http://example.com | ./search`

### 课堂讨论

- 这个简单的 href 提取器有什么局限性？(如：`href = "..."` 中间有空格)
- 搜索引擎的爬虫和本程序有什么关系？
- 如何扩展为支持递归爬取 (提取链接 → 下载 → 再提取)？

### 课后练习

- 用 `wget -O - http://example.com | ./search` 测试真实网页
- 添加过滤：只输出以 `http` 开头的绝对 URL
- 挑战：添加去重功能 — 用散列表记录已见过的 URL
