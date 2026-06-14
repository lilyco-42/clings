## Lesson 55 Memory Allocator 内存分配器

### 代码
	typedef struct Block {
	    size_t size;         /* payload 大小 */
	    int free;            /* 1=空闲, 0=已分配 */
	    struct Block *next;  /* 链表下一块 */
	} Block;
	
	void *my_malloc(size_t size)
	{
	    size = align_up(size);
	    Block *cur = free_list;
	    while (cur) {
	        if (cur->free && cur->size >= size) {
	            split_block(cur, size);  /* 拆分过大的块 */
	            cur->free = 0;
	            return (char *)cur + HEADER_SIZE;
	        }
	        cur = cur->next;
	    }
	    return NULL; /* OOM */
	}

### 知识点
* 内存分配器核心数据结构
	- **Block header**: 每块前面存 `{size, free, next}` 元数据
	- **Free list**: 所有块组成单链表, 遍历查找空闲块
	- 用户拿到的指针 = block 地址 + HEADER_SIZE
* 分配策略: First-Fit
	- 遍历链表, 找到第一个够大的空闲块
	- 优点: 简单, 分配速度较快
	- 缺点: 容易产生外部碎片
* 块分割 (Split)
	- 空闲块远大于请求时, 拆成"已分配"+"新空闲"两块
	- 避免浪费: 剩余空间 >= ALIGN 才拆分
* 合并 (Coalesce)
	- `my_free` 后扫描相邻空闲块, 合并为更大的块
	- 防止碎片累积: 连续 free 不会产生多个小碎片
* 对齐 (Alignment)
	- 所有分配按 8 字节对齐 (`align_up`)
	- 确保返回的指针满足任意类型的对齐要求

### 课堂讨论
* 为什么 `malloc` 返回的指针前面要有 header？`free` 怎么知道大小？
* Best-Fit vs First-Fit vs Next-Fit 各有什么优缺点？
* 真实的 glibc malloc (ptmalloc2) 用了哪些更复杂的技术？

### 课后练习
* 添加 `my_calloc(count, size)`: 分配并清零
* 实现 Best-Fit 策略, 对比碎片情况
* 挑战: 用 `sbrk()` 替代静态数组, 实现真正的堆扩展

### 参考资料
* K&R《C程序设计语言》8.7节: 存储分配程序
* "A Memory Allocator" by Doug Lea http://gee.cs.oswego.edu/dl/html/malloc.html
