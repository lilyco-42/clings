## Lesson 51 Run-Length Encoding 游程编码压缩

### 代码
	/* RLE编码核心: 连续相同字节压缩为 [count][byte] */
	long rle_encode(FILE *in, FILE *out)
	{
	    int cur = fgetc(in);
	    long total = 0;
	    while (cur != EOF) {
	        int count = 1;
	        int next;
	        while ((next = fgetc(in)) != EOF && next == cur && count < 255)
	            count++;
	        fputc(count, out);
	        fputc(cur, out);
	        total += 2;
	        cur = next;
	    }
	    return total;
	}

### 知识点
* RLE 编码原理
	- 将连续重复的字节序列压缩为 `[长度][字节]` 对
	- 适合: 大量连续重复数据 (如位图、日志)
	- 不适合: 随机数据 (最坏情况 2 倍膨胀)
* 编码设计
	- 计数器范围 1-255 (单字节), 超过 255 则拆分
	- 流式处理: 一次只读一个字节, 内存需求恒定 O(1)
* 文件 I/O 的二进制模式
	- `"rb"` / `"wb"`: 二进制读写, 不做换行转换
	- `fgetc` 返回 `int`: 区分 EOF (-1) 和有效字节 (0-255)

### 课堂讨论
* RLE 对 "ABABABAB" 这种交替数据压缩效果如何？
* PackBits 变体: 如何用负数表示"接下来 N 个不同字节"？
* 为什么 BMP 位图格式使用 RLE 而 JPEG 不用？

### 课后练习
* 对一个文本文件进行 RLE 编码, 观察压缩率
* 实现 PackBits 变体: 正数=重复, 负数=逐字节
* 对比 Lesson 52 LZ77 的压缩率

### 参考资料
* PackBits 算法 https://en.wikipedia.org/wiki/PackBits
* BMP 文件格式的 RLE 压缩段
