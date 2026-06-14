## Lesson 52 LZ77 Sliding Window Compression 滑动窗口压缩

### 代码
	/* 在滑动窗口中查找最长匹配 */
	static int find_match(const unsigned char *data, int pos, int len,
	                      int *out_offset, int *out_length)
	{
	    int best_off = 0, best_len = 0;
	    int search_start = (pos > WINDOW_SIZE) ? pos - WINDOW_SIZE : 0;
	    for (int i = search_start; i < pos; i++) {
	        int mlen = 0;
	        while (mlen < MAX_MATCH && pos + mlen < len &&
	               data[i + mlen] == data[pos + mlen])
	            mlen++;
	        if (mlen > best_len) { best_len = mlen; best_off = pos - i; }
	    }
	    *out_offset = best_off; *out_length = best_len;
	    return best_len >= 3;
	}

### 知识点
* LZ77 算法核心思想
	- **滑动窗口**: 已处理的数据作为"字典"(4KB)
	- **前向缓冲区**: 待压缩数据 (15字节)
	- 输出: `(偏移, 长度)` 或 `(0, 字面量字节)`
* 编码格式设计
	- 2 字节 token: `[offset_hi:4|len:4][offset_lo:8]`
	- 4096 窗口 = 12位偏移, 15 最大匹配 = 4位长度
	- 仅编码 >= 3 字节的匹配 (否则膨胀)
* 与 RLE 的对比
	- RLE: 只压缩连续重复 (AAAA → 4A)
	- LZ77: 压缩任意重复模式 (ABCABC → ABC + 回引)

### 课堂讨论
* 为什么窗口大小选 4096？增大窗口有什么代价？
* DEFLATE (zip/gzip) = LZ77 + Huffman, 为什么要组合？
* 解压是否可以流式进行？内存需求是多少？

### 课后练习
* 测试不同窗口大小对压缩率的影响
* 对比: 先 RLE 后 LZ77 vs 直接 LZ77
* 挑战: 实现 LZSS 变体 (使用标志位区分字面量和匹配)

### 参考资料
* Ziv & Lempel, "A Universal Algorithm for Sequential Data Compression", 1977
* RFC 1951 - DEFLATE 压缩格式规范
