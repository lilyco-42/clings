## Lesson 16 How to strcpy 字符串拷贝

    #include <stdio.h>
    #include <assert.h>

    #if 1
    char * my_strcpy(char * dst, const char * src)
    {
    	assert(dst != NULL && src != NULL);

    	char * ret = dst;

    	while ((*dst++ = *src++) != '\0');

    	return ret;
    }
    #else
    char * my_strcpy(char * dst, const char * src)
    {
    	char * ret = dst;

    	while((*dst++ = *src++) != '\0');

    	return ret;
    }
    #endif

    int main(int argc, char *argv[])
    {
    	char d[32];
    	char s[32] = "Hello World";

    	printf("s = %s\n", s);

    //	my_strcpy(d, s);
    	printf("d = %s\n", my_strcpy(d, s));

    	return 0;
    }

### 算法优化 1 - 减少对 dst 指针变量的存取次数

    #include <stdio.h>

    char * my_strcpy(char * dst, const char * src)
    {
    	assert(dst != NULL && src != NULL);

    	char *s = (char *)src;
    	int delt = dst - src;

    	while ((s[delt] = *s++) != '\0');

    	return dst;
    }

出处：林锐《高质量程序设计指南》<http://www.360doc.com/content/11/0426/13/6845720_112422873.shtml>

### 算法优化 2 - 内联函数和内嵌汇编

    extern inline void * memcpy(void * dest,const void * src, int n)
    {
    	__asm__("cld\n\t"
    	        "rep\n\t"
    	        "movsb"
    	        ::"c" (n),"S" (src),"D" (dest)
    	        :"cx","si","di");
    	return dest;
    }

出处：Linux 0.0.1 源码 <http://lxr.linux.no/linux-old+v0.01/include/string.h>

### 算法优化 3 - 基于 PAGE/WORD/BYTE 对齐拷贝

    #include <string.h>
    #include <memcopy.h>
    #include <pagecopy.h>

    #undef memcpy

    void *
    memcpy (dstpp, srcpp, len)
         void *dstpp;
         const void *srcpp;
         size_t len;
    {
      unsigned long int dstp = (long int) dstpp;
      unsigned long int srcp = (long int) srcpp;

      /** Copy from the beginning to the end.  */

      /** If there not too few bytes to copy, use word copy.  */
      if (len >= OP_T_THRES)
        {
          /** Copy just a few bytes to make DSTP aligned.  */
          len -= (-dstp) % OPSIZ;
          BYTE_COPY_FWD (dstp, srcp, (-dstp) % OPSIZ);

          /** Copy whole pages from SRCP to DSTP by virtual address manipulation,
    	 as much as possible.  */

          PAGE_COPY_FWD_MAYBE (dstp, srcp, len, len);

          /** Copy from SRCP to DSTP taking advantage of the known alignment of
    	 DSTP.  Number of bytes remaining is put in the third argument,
    	 i.e. in LEN.  This number may vary from machine to machine.  */

          WORD_COPY_FWD (dstp, srcp, len, len);

          /** Fall out and copy the tail.  */
        }

      /** There are just a few bytes to copy.  Use byte memory operations.  */
      BYTE_COPY_FWD (dstp, srcp, len);

      return dstpp;
    }
    libc_hidden_builtin_def (memcpy)

### 算法优化 4 - 基于 4 字节对齐的拷贝

    #define UNALIGNED(x,y) (((unsigned long)x & (sizeof (unsigned long)-1)) ^ ((unsigned long)y & (sizeof (unsigned long)-1)))
    #define STRALIGN(x) (((unsigned long)x&3)?4-((unsigned long)x&3):0)
    # define MKW(x) (x|x<<8|x<<16|x<<24)
    # define GFC(x) ((x)&0xff)
    # define INCSTR(x) do { x >>= 8; } while (0);

    char *
    strcpy (char *s1, const char *s2)
    {
        char           *res = s1;
    #ifdef WANT_SMALL_STRING_ROUTINES
        while ((*s1++ = *s2++));
        return (res);
    #else
        int             tmp;
        unsigned long   l;

        if (UNALIGNED(s1, s2)) {
        while ((*s1++ = *s2++));
        return (res);
        }
        if ((tmp = STRALIGN(s1))) {
        while (tmp-- && (*s1++ = *s2++));
        if (tmp != -1) return (res);
        }

        while (1) {
        l = *(const unsigned long *) s2;
        if (((l - MKW(0x1ul)) & ~l) & MKW(0x80ul)) {
            while ((*s1++ = GFC(l))) INCSTR(l);
            return (res);
        }
        *(unsigned long *) s1 = l;
        s2 += sizeof(unsigned long);
        s1 += sizeof(unsigned long);
        }
    #endif
    }

出处：glibc 库函数实现

- <http://www.oschina.net/code/explore/glibc-2.9/string/strcpy.c>
- <http://www.oschina.net/code/explore/glibc-2.9/string/memcpy.c>
- 汇编实现--基本思路是实现好 1,2,4,8…字节的硬代码，根据实际字节数做跳转，循环前根据 cache line 状态做 prefetch  
  <http://www.oschina.net/code/explore/glibc-2.9/sysdeps/x86_64/memcpy.S>

### 尚未看懂的优化实现 - 内核 3.6.7 代码实现

- <http://lxr.linux.no/linux+v3.6.7/arch/arm/lib/copy_template.S>
- <http://lxr.linux.no/linux+v3.6.7/arch/arm/lib/memcpy.S>

### 知识点

- 指针
  - 字符指针
- 字符指针 vs 字符数组

### 课堂讨论

- 当拷贝函数参数 dst 和 src 内容重叠时应该怎么办呢？
  - 参考阅读 <http://blog.csdn.net/hzgdiyer/article/details/6163767>
- 考虑到内存数据总线是 32 位，每 4 字节进行读写内存时候的效率最高，程序应该怎么改？
  - 参考阅读 <http://blog.csdn.net/wind19/article/details/7539027>
  - <http://blog.sina.com.cn/s/blog_6ba6e53b010145bz.html>
  - libc 代码分析 <http://www.cnblogs.com/egmkang/archive/2010/05/25/1743267.html>
- char s[32] = "Hello World"; 修改为 char \* s = "Hello World"; 有何区别？
  - 思路提示：从两者的存储分配，读写权限，初始化实现，执行时效率进行分析
- 怎样写出一个标准规范的 strcpy 函数？
  - 编程风格
  - 出错处理
  - 算法效率（用于提高性能）

### 课后练习

- 请用指针实现对一个数组的调整，要求奇数在左边，偶数在右边。要求：尽可能不占用额外的存储空间。
- 请结合 strcpy 的实现，给出 strlen 的实现。要求：考虑内存对齐和算法执行效率。

# 新浪微博大讨论

## @Tan_yb

对@亚嵌李明老师 说：老师我发现一个问题：内核和 glibc 的 memcpy 比我们自已写的性能高这么多。

Tan_yb：参考 http://t.cn/zjyVHFq 进行了优化 http://t.cn/zjyVHFG memcpy 执行效率果然快了很多，我发现定义的结构体 size 达到 64 个字节就应该是效率执行的最大化。

## @亚嵌李明老师

glibc 里面对内存的读写考虑了对齐问题，一般是以字 WORD 对齐进行整块拷贝，效率应该能够提高 4 倍左右。另外也考虑了以页为单位进行整页拷贝。详见 http://t.cn/zjyyuF5 http://t.cn/zjyyuFt

## @Stanley 文威

memcpy 可是软件优化和 CPU 架构相结合的典型例子。WORD 对齐，cacheline 对齐，预取等等都在里面，我记得我有好几个同事优化 memcpy 优化了好久。属于计算机里的微雕艺术。//@翁恺 BA5AG: 呵呵，昨天求计的 C 课正好讲到这个，要强迫他们养成凡事尽量使用现成代码的习惯

关于体系架构和软件优化，还有个故事。以前某天某公司的某个加速库就要发布了，大家发现性能降了 20%，大家都抓狂了。这种加速库被优化了 n 年，各个边边角角被优化了好几遍，20% 的性能下降简直是不可想像的，于是大家郁闷的加班找原因。

最后发现玄机在 Makefile 里，Makefile 里最关键的一句就是$(CC) -o xxx.a a.o b.o c.o ... z.o, 一个工程师顺手把这行改成$(CC) -o xxx.a a.o c.o b.o ... z.o !?! 原来这样一行修改改变了某几个函数的内存位置，恰巧让他们占用了同几个 I$的 slot，于是 I$ miss 飙高，性能下降。（continue）

这时候大家才知道为了避免不必要的 i$ miss，第一个写这个 Makefile 的姐姐花了很长时间来人肉调整 object 文件的次序。这可是全排列哦。这 JJ 当时已经离开公司了，只留下美丽的传说。。。。再后来我们好像在 gcc 里做了这种优化，再具体的细节我也忘了，你懂的

## @宋宝华 Barry

memcpy 非常讲究，要充分考虑指令集，cacheline,word 对齐等。可看 arch/arm/lib/memcpy.S 和 copy_template.S//@翁恺 BA5AG: //@Stanley 文威：memcpy 可是软件优化和 CPU 架构相结合的典型例子。WORD 对齐，cacheline 对齐，预取等等都在里面，我记得我有好几个同事优化 memcpy 优化了好久。属于计算机里的微雕艺术。 (11 月 21 日 22:07)

宋宝华 Barry：回复@Tan_yb: 方法错。memcpy 没人拿 C 语言搞的，都是根据每个 arch 写汇编。你用 C，就依赖于 toolchain 是否能转化为正确指令集。//@Tan_yb：参考 http://t.cn/zjyVHFq 进行了优化 http://t.cn/zjyVHFG memcpy 执行效率果然快了很多，我发现定义的结构体 size 达到 64 个字节就应该是效率执行的最大化

宋宝华 Barry：前一段很准确，后一段属误解。内核 dma 的 memcpy 发生于 io 与 mem 或者 GPU 等的 mem 与 mem，不发生于典型 memcpy，而且目的也不是 cache miss 更少，更不一定是加速。//@翁恺 BA5AG: //@CSK\_: x86 上的 memcpy 库函数会使用 SSE 指令加速，arm 上的会用 NEON 指令加速。内核态甚至会用 DMA 加速，同时 cache miss 也小

## @CSK\_

x86 上的 memcpy 库函数会使用 SSE 指令加速，arm 上的会用 NEON 指令加速。内核态甚至会用 DMA 加速，同时 cache miss 也小//@翁恺 BA5AG: 不同的 CPU 实现肯定不同，x86 的和 ARM 不会一样 //@宋宝华 Barry:memcpy 非常讲究，要充分考虑指令集，cacheline,word 对齐等。可看 arch/arm/lib/memcpy.S 和 copy_template.S

## @蔡华林 CHL

回复@亚嵌李明老师：可以找找 cache miss 和 false sharing 相关的文章看看，当你的程序有大量循环时一不小心就会遇上，可以通过 vtune 之类工具发现。现在 cpu 是很快，显得内存访问的 latency 越来越无法忍受了，貌似是几乎上百纳秒级别，一次内存装载到 cache 的时间够执行很多语句了

但是一般人写程序不会注意到这个问题吧，前段时间想改进 hash join，每次算好哈希入口去访问都会引起一次 cache miss，性能就只有遍历的 1/10，于是想显式发预取指令，这样得弄个窗口处理当前项时预取下一项，由于之前设计问题缓存会增加额外的 memcpy 和组装开销，抵消掉大部分预取带来的好处…

我以前还专门被叫去研究过这个函数，在某些特殊情况下能做到自己写的更快，因为可以被 inline 掉，前提是对齐之类要注意，改写成 word 的赋值语句，还有些情况下应该使用 gcc 内置的 builtin 的 memcpy 或者 vc 的 intrinsic，这样编译器也能帮省掉函数调用开销

glibc 2.9 有个 500 多行的 memcpy 汇编，这里有源码：http://t.cn/zjUwD43，不知道最新的代码有没有改过，我电脑上 07 年载的已经是这个版本了，基本思路应该就是实现好 1,2,4,8…字节的硬代码，根据实际字节数做跳转，循环前根据 cache line 状态做 prefetch

## @ChongHead

X86 的 glibc memcpy 大致有 4k 行汇编代码，性能差别不是一点点的，牛人作者在上海 //@CSK\_: x86 上的 memcpy 库函数会使用 SSE 指令加速，arm 上的会用 NEON 指令加速。内核态甚至会用 DMA 加速，同时 cache miss 也小//@翁恺 BA5AG: 不同的 CPU 实现肯定不同，x86 的和 ARM 不会一样 //@宋宝华 Barry:memcpy

## @曦图吴昊

我们以前做这种优化都是根据 CPU 的 cycle 设计算法的 //@CSK\_:x86 上的 memcpy 库函数会使用 SSE 指令加速，arm 上的会用 NEON 指令加速。内核态甚至会用 DMA 加速，同时 cache miss 也小 //@翁恺 BA5AG: 不同的 CPU 实现肯定不同，x86 的和 ARM 不会一样

## @孙志岗 Sunner

从 memcpy 到 filecopy 的优化，足够学到好大一箩筐了//@Stanley 文威：memcpy 可是软件优化和 CPU 架构相结合的典型例子。WORD 对齐，cacheline 对齐，预取等等都在里面，我记得我有好几个同事优化 memcpy 优化了好久。属于计算机里的微雕艺术。//@翁恺 BA5AG: 要强迫他们养成凡事尽量使用现成代码的习惯
