## Lesson 40 - MyDictionary 项目 (查字典程序)

### 课程任务

用散列表 (Hash Table) 实现英汉字典查询程序，综合运用 Lesson 33-39 所学的数据结构知识。

- 运行示例

      $ ./dict
      Loading dictionary from 'dict.txt'...
      Loaded 10 entries.
      Entries: 10, Buckets used: 10/10007 (0.1%), Max chain: 1

      Enter a word to look up (Ctrl+D to quit):
      > hello
        hello: 你好; 打招呼
      > compile
        compile: 编译; 汇编

### 代码

    /* djb2 哈希函数 */
    static unsigned int hash(const char *s)
    {
        unsigned int h = 5381;
        while (*s)
            h = h * 33 + (unsigned char)*s++;
        return h % HASH_SIZE;
    }

    /* 查找单词 */
    static const char *dict_lookup(const char *word)
    {
        unsigned int h = hash(word);
        for (Entry *e = table[h]; e; e = e->next)
            if (strcmp(e->word, word) == 0)
                return e->definition;
        return NULL;
    }

### 重要知识点

- 散列表 (Hash Table) 实现
  - djb2 哈希函数：`h = h * 33 + c`，简单高效
  - 链式冲突解决：每个桶是一个链表
  - 表大小选素数 (10007): 减少哈希碰撞
- 词典数据加载
  - 从文件逐行读取：`word<TAB>definition` 格式
  - 支持命令行指定词典文件或使用内置 demo
- 统计信息
  - `dict_stats()`: 输出桶利用率和最长链长度
- 内存管理
  - `malloc` 分配 Entry 节点，`dict_free()` 遍历释放

### 课堂讨论

- djb2 中为什么选 33 作为乘数？换成 31 或 37 效果如何？
- 如果词典有 100 万词，表大小应该设多少？
- 对比：散列表 O(1) vs 二叉搜索树 O(log n)，各自优劣？

### 课后练习

- 用实际词典文件测试，统计不同 HASH_SIZE 下的冲突率
- 添加模糊查询：输入前缀匹配所有以此开头的单词
- 挑战：实现开放寻址法 (Open Addressing) 替代链式法

### 参考资料

- K&R《C 程序设计语言》6.6 节 表查找
- Hash Function Evaluation http://www.cse.yorku.ca/~oz/hash.html

---

以下为原始项目完整规格说明 (供拓展参考):

# 项目说明 (完整版)

    给定文本文件“dict.txt”，该文件用于存储词库。词库为“英-汉”，“汉-英”双语词典，每个单词和其解释的格式固定，如下所示：

    	#单词
    	Trans:解释1@解释2@…解释n

    每个新单词由“#”开头，解释之间使用“@”隔开。一个词可能有多个解释，解释均存储在一行里，行首固定以“Trans：”开头。

    下面是一个典型的例子：

    	#abyssinian
    	Trans:a. 阿比西尼亚的@n. 阿比西尼亚人;依索比亚人

    该词有两个解释，一个是“a. 阿比西尼亚的”；另一个是“n. 阿比西尼亚人;依索比亚人”。

- 词库文件下载 [dict.txt](https://dl.dropboxusercontent.com/u/63667688/dict.txt)

# 项目要求

### 1. 单词查询

    要求编写程序将词库文件读取到内存中，接受用户输入的单词，在字典中查找单词，并且将解释输出到屏幕上。
    用户可以反复输入，直到用户输入“exit”字典程序退出。

    程序执行格式如下所示：
    ./mydict
    	默认使用 dict.txt 文本词库进行单词查找。
    ./mydict -f mydict.txt
    	表示使用文本词库 mydict.txt 文件进行单词查找。

_用户输入单词的前 n 个字母，可以作为模糊查询，提示用户匹配的单词。_

### 2. 支持用户自添加新词/删除单词

    用户添加的新词存放在指定文件中。如果待查单词在词库中找不到，则使用用户提供的词库。用户的词库使用文本形式保存，便于用户修改。

    程序执行格式如下所示。
    ./mydict --add/del -f mydict.txt
    --add/del 表示进入删除或者添加模式，-f为固定参数，用来指定用户词库文件名.

### 3. 查询单词的词频统计

    用户查询过程中，记录查询单词和查询次数，便于用户进行单词词频的统计。

    程序执行格式如下所示。
    ./mydict --search-sort

    按照查询次数从多到少依次列出用户曾经查询过的单词和查询次数。

- 可以在此基础上，实现一个简单的生词记忆程序，帮助用户背记经常查询的生词\*

### 4. 网页翻译

    用户指定一个web页面的链接，程序对网页上面的正文内容进行翻译，并输出到一个文本文件中。

    程序执行格式如下所示。
    ./mydict --translate http://www.google.com.hk/intl/en/about/
    ./mydict --translate http://www.kernel.org -o result.html

# 技术参考

### ncurses 库 - 实现 Linux 终端上的图形界面编程

    参考资料： google  "NCURSES Programming HOWTO中文版"
    安装： sudo apt-get install ncurses-dev
    使用： gcc your_program.c -lncurses

### wget 命令 - 网页抓取工具

    参考资料： man wget

### system 调用 - 实现 C 程序中调用 shell 命令

    参考资料： man system

# 考察知识点

    （1）	变量数据类型
    （2）	数组
    （3）	结构体
    （4）	typedef关键字的使用
    （5）	控制结构
    （6）	函数接口设计
    （7）	static关键字的使用
    （8）	文件拆分与代码组织
    （9）	模块化设计思想
    （10）	简单的面向对象程序设计思想
    （11）	指针与指针控制
    （12）	const关键字的使用
    （13）	C语言程序的命令行参数
    （14）	多文件符号解析
    （15）	头文件包含
    （16）	宏
    （17）	条件编译
    （18）	字符串操作
    （19）	malloc函数
    （20）	常用的字符串库函数
    （21）	文件操作
    （22）	简单的出错处理
    （23）	排序算法和二分查找算法
    （24）	二进制文件和文本文件的区别
    （25）	链表操作
    （26）	复杂链表的链表操作
    （27）	二叉树
    （28）	makefile的使用
    （29）	编程工具的使用（vi，gcc，gdb）
    （30）	文档组织和项目规划
