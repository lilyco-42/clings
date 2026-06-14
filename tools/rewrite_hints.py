#!/usr/bin/env python3
"""Rewrite all Unit 1 hints to Rustlings style."""
import tomllib
import re
from pathlib import Path

EXERCISES_DIR = Path(__file__).resolve().parent.parent / "exercises"

# New hints keyed by exercise name
NEW_HINTS = {
    # Lesson 01
    "01a_return_zero": """\
最简单的 C 程序只需要一个 main 函数。
编译器报错了吗？读读错误信息——它告诉你缺少什么。

一个完整的函数定义需要:
- 返回类型 (整数类型)
- 函数名 (C 程序的入口点叫什么？)
- 参数列表 (无参数用 void 表示)
- 函数体 { ... }

返回 0 表示程序正常退出。

参考: 本课 README 中的 BNF 范式分析。""",

    "01b_return_expression": """\
这道题要求你定义变量并计算表达式。

思考:
- 全局变量定义在 main 函数外面
- 局部变量定义在 main 函数里面
- return 后可以跟一个表达式，不一定只是数字

期望返回值是 3。想想什么简单的加法能得到 3？

参考: 同目录下的 variable.c 示例。""",

    # Lesson 02
    "02a_hello_printf": """\
要在屏幕上打印东西，你需要用到 printf 函数。

但 printf 不是 C 语言内置的——你需要先"导入"它。
怎么导入？用 #include 和一个头文件。printf 住在哪个头文件里？

注意输出的确切内容:
- 全部小写
- 句末有句号
- 末尾需要换行 (\\n 表示换行符)

参考: 本课 README 中关于预处理符和头文件的说明。""",

    "02b_format_print": """\
printf 可以用"格式说明符"把变量的值嵌入字符串中打印。

常用格式符:
- %d → 十进制整数
- %x → 十六进制 (小写 a-f)
- 0x%x → 带 0x 前缀的十六进制

看看期望输出的格式: "local = 100" 和 "global = 0xc8"。
100 的十六进制是什么？想想 16*12+8。

参考: 本课 README 中的格式化输出说明。""",

    # Lesson 03
    "03a_while_loop": """\
while 循环的基本结构: while (条件) { 循环体 }

你需要:
1. 一个计数变量，从 1 开始
2. 循环条件: 什么时候停下来？(到 10 为止)
3. 每次循环: 打印当前值，然后让计数器增加

思考 counter++ 应该在打印之前还是之后？
看看期望输出的第一行是 "counter = 1" 还是 "counter = 0"。

参考: 本课 README 中的 while 循环示例。""",

    "03b_for_loop": """\
for 循环的三个部分: for (初始化; 条件; 更新) { ... }

这次要倒着数! 从 10 数到 1。思考:
- 初始化: counter 从哪个数开始？
- 条件: 什么时候停止？
- 更新: 每次减少多少？(counter--)

注意期望输出的最后一行是 "sum = 55"——
你还需要在循环中累加每个 counter 的值。

参考: 本课 README 课后练习部分。""",

    # Lesson 04
    "04_odd_or_even": """\
如何判断一个数是奇数还是偶数？

取余运算符 % 可以告诉你除法的余数。
一个数除以 2，余数是 0 还是 1？这就区分了奇偶。

用 if-else 根据判断结果打印不同的信息。
看看期望输出的格式: "num 7 is odd" 和 "num 12 is even"。

别忘了先用 scanf 读取输入的整数。

参考: 本课 README。""",

    # Lesson 05
    "05a_sum_1_to_100": """\
这道题要你计算 1+2+3+...+100 的和。

想想: 你需要什么变量来"累积"结果？
循环从哪里开始到哪里结束？每次循环做什么操作？

提示: 高斯小时候用 n*(n+1)/2 算出了答案，
但这里我们要用循环来逐个累加。最终结果是 5050。

参考: 本课 README。""",

    "05b_sum_even": """\
这道题在求和的基础上加了一个条件: 只加偶数。

C 语言有一个关键字可以"跳过"循环的当前迭代。
当遇到不想处理的数时，用它直接进入下一轮循环。

怎么判断一个数不是偶数？回忆一下 % 运算符。

参考: 本课 README 中 continue 的用法。""",

    # Lesson 06
    "06_multiplication_table": """\
九九乘法表需要两层嵌套循环——一个循环控制行，一个控制列。

思考:
- 外层循环 i 代表被乘数 (1 到 9)
- 内层循环 j 代表乘数 (从 1 到 i)
- 每项格式是 "j*i=结果"，用制表符 \\t 分隔
- 每行结束后换行

仔细观察期望输出中 j 和 i 的顺序——是 j*i 不是 i*j。

参考: 本课 README 中的嵌套循环说明。""",

    # Lesson 07
    "07a_max_prime": """\
如何判断一个数是不是素数 (质数)？

素数是只能被 1 和自身整除的正整数。
判断 n 是否为素数: 从 2 开始试除，如果能整除就不是素数。
优化: 你只需要试除到 sqrt(n) 就够了——想想为什么？

你需要 math.h 中的 sqrt 函数，注意类型转换。

从 100 往回找，找到的第一个素数就是答案。

参考: 本课 README 中的 sqrt 和三层逻辑嵌套。""",

    # Lesson 08
    "08_count_digit_nine": """\
统计 1 到 100 中数字 9 出现了多少次。

关键: 把问题分解成两个子问题:
1. 怎么从一个数中提取每一位数字？(除以 10 取余)
2. 怎么遍历 1 到 100？

试试设计一个函数 find(num, digit) 来检查某个数字中
包含多少个指定的 digit。用 do-while 循环逐位提取。

注意: 99 中包含两个 9！

参考: 本课 README 中关于函数设计和 do-while 的说明。""",

    # Lesson 09
    "09a_itoa_hex": """\
整数转十六进制字符串——和十进制的 itoa 思路一样，
只是基数从 10 变成了 16。

关键思路:
1. 每次取 num % 16 得到一位 (0-15)
2. 用一个映射表 "0123456789ABCDEF" 把数字转成字符
3. num /= 16 继续处理下一位
4. 注意: 这样得到的是逆序的！最后需要反转

为什么用 do-while 而不是 while？
因为当 num=0 时我们也需要输出 "0"。

参考: 本课 README。""",

    "09_integer_to_string": """\
把一个整数转换成字符串——这就是 itoa 函数的工作。

算法:
1. 反复取余 (num % 10) 得到最低位数字
2. 加上 '0' 变成字符 (ASCII: '0'=48, '1'=49...)
3. 除以 10 去掉最低位
4. 重复直到 num 为 0

但这样存入数组的是逆序的! 你需要一个 reverse 函数。

别忘了字符串末尾要加 '\\0' 终止符。

参考: 本课 README 中的数组和字符串逆序。""",

    # Lesson 10
    "10a_init_ring": """\
约瑟夫环的第一步: 用数组模拟循环链表。

next[i] 表示第 i 个人的下一个人是谁。
如果有 n 个人围成一圈:
- 第 0 个人的下一个是第 1 个
- 第 1 个的下一个是第 2 个
- ...
- 第 n-1 个的下一个是第 0 个 (形成环!)

有什么运算能让 n-1 的下一个变成 0？想想取模运算。

参考: 同目录下的原课 josephus.c。""",

    "10_josephus_ring": """\
约瑟夫环: n 个人围成圈，每数到第 m 个人出局。

核心循环:
1. 用一个计数器记录当前数到第几个
2. 沿着 next[] 链表前进
3. 当计数器达到出局数时——这个人出局!
4. "出局"在链表中怎么实现？让前一个人直接跳过他
5. 重复直到所有人都出局

想想: 你需要记录"前一个人"是谁 (prev)，
这样才能在出局时修改链表: next[prev] = next[i]。

参考: 同目录下的原课 josephus.c。""",

    # Lesson 11
    "11_point_distance": """\
求两个点之间的距离。

你需要:
1. 定义一个结构体来表示"点" (有 x 和 y 两个坐标)
2. 用勾股定理计算距离: sqrt(dx*dx + dy*dy)
3. 需要 math.h 中的 sqrt 函数

C 语言中，结构体可以直接作为函数参数传递。
输出用 %.2f 保留两位小数。

参考: 本课 README 中的 struct 和 typedef。""",

    # Lesson 12
    "12_little_endian": """\
判断你的机器是大端还是小端字节序。

什么是字节序？一个多字节的整数 (如 int) 在内存中，
低位字节在前 (小端) 还是高位字节在前 (大端)？

怎么检测？把一个已知值存入 int，然后看它的第一个字节。

方法一: 用 union 让 int 和 char 共享同一块内存。
方法二: 用指针将 int* 转成 char*，读取第一个字节。

return 1 表示小端 (Little Endian)，return 0 表示大端。

参考: 本课 README 中的 union 和字节序说明。""",

    # Lesson 13
    "13a_get_last_char": """\
如何获取一个字符串的最后一个字符？

C 字符串以 '\\0' 结尾。你需要遍历到末尾。

思路: 一个简单的 while 循环，记录每个字符，
直到遇到 '\\0'。循环结束时，你记录的就是最后一个字符。

参考: 同目录下的原课 restrict.c 中的 get_last_char()。""",

    "13b_is_restricted": """\
车辆限行规则: 根据车牌尾号和今天的星期，判断是否限行。

这是 switch-case 的经典应用:
- 尾号 0/5 → 周一限行
- 尾号 1/6 → 周二限行
- ...

函数返回值用三元表达式可以很简洁:
(条件) ? 1 : 0

参考: 同目录下的原课 restrict.c 中的 is_restricted()。""",

    "13c_restrict": """\
这道综合题要你把三个函数组装起来:
1. get_last_char(): 取车牌最后一位 (已在 13a 实现)
2. is_restricted(): 判断是否限行 (已在 13b 实现)
3. get_week_day(): 根据日期计算星期几

get_week_day 的难点: 如何从年月日算出星期？
关键是算出这一天距离某个参考日 (已知星期) 的总天数，
然后对 7 取余。

参考: 同目录下的原课 restrict.c 中的完整实现。""",

    # Lesson 14
    "14a_print_board": """\
用二维数组表示一个 5x5 的棋盘，并打印它。

两个任务:
1. init_chessboard(): 用 my_rand() % 2 随机填充
   双重 for 循环遍历每个位置

2. print_chessboard(): 打印棋盘
   外层循环遍历行，内层遍历列
   元素之间用空格隔开，每行末尾换行

参考: 同目录下的原课 solution.c。""",

    "14b_dfs": """\
判断棋盘上某个位置有几个方向可以走。

check 函数接收当前位置和一个方向:
1. 计算新位置: nr = row + 方向.dr, nc = col + 方向.dc
2. 检查新位置是否在棋盘内 (用 is_valid)
3. 检查新位置的值是否为 0 (可通行)

两个条件都满足才返回 1 (可以走)。

struct direction 已经定义好了四个方向 (上下左右)，
每个方向有 dr 和 dc 两个偏移量。

参考: 同目录下的原课 solution.c 中的 struct direction。""",

    # Lesson 15
    "15a_count_bits_loop": """\
统计一个整数的二进制表示中有多少个 1。

方法一: 逐位检测。

整数有 32 位。逐个检查每一位:
- 用 1 左移 i 位得到一个只有第 i 位为 1 的掩码
- 用 & 运算检查原数的第 i 位是否为 1

循环 32 次，统计有多少位是 1。

参考: 本课 README 中的位运算方法一。""",

    "15b_count_bits_clear": """\
统计二进制中 1 的个数——更聪明的方法。

方法二: n & (n-1) 的魔法。

这个操作能做什么？它会清除 n 的最右边的那个 1！
例如: 1100 & 1011 = 1000 (最右边的 1 被清掉了)

所以: 不断执行 n &= (n-1)，每次消除一个 1，
直到 n 变成 0。执行了多少次就有多少个 1。

参考: 本课 README 中的位运算方法二。""",

    "15_count_bits": """\
统计二进制中 1 的个数——分治法 (popcount)。

方法三: 用 5 层掩码把每一"段"的 1 加起来。

这是 gcc __builtin_popcount 的底层原理:
- 第 1 层: 每 2 位一组，相邻位相加
- 第 2 层: 每 4 位一组
- 第 3 层: 每 8 位一组
- 第 4 层: 每 16 位一组
- 第 5 层: 整个 32 位

关键是定义好 5 个掩码 M1-M5。
这道题有挑战性——如果想不出来，先看看 README 的图解。

参考: 本课 README 中的分治法详解。""",

    # Lesson 16
    "16a_strcpy_delta": """\
这是 strcpy 的一个优化版本——delta 技巧。

核心思想: 计算 dst 和 src 之间的偏移量 delta，
然后只用一个指针就能同时访问源和目标。

这是一个来自《高质量程序设计指南》的经典优化。
想想: 如果你知道 dst 在 src 后面 delta 个字节，
那 src[delta] 就是 dst 对应位置。

注意: 这道题的 src 和 dst 在同一个缓冲区内。

参考: 本课 README。""",

    "16_my_strcpy": """\
实现自己的字符串复制函数 mystrcpy。

C 程序员的经典写法:
  while (条件) { 复制一个字符; 移动指针; }

更简洁的 C 惯用法是把复制和条件判断合并在一起。
想想: 赋值表达式本身也有返回值!

什么时候停止复制？当遇到字符串的终止符 '\\0' 时。

参考: 本课 README 中关于指针和 const 的说明。""",

    # Lesson 17
    "17_word_count": """\
统计文本中有多少个单词。

关键问题: 怎么知道一个"单词"开始了？

状态机思想: 用一个变量记录当前状态——
你是在单词"里面"还是"外面"？

- 看到字母且当前在"外面" → 一个新单词开始了!
- 看到空白且当前在"里面" → 单词结束
- 其他情况 → 状态不变

每次发现新单词的"起点"时，计数器加一。
同时需要打印每个单词的字符。

参考: 本课 README 中的"状态机编程思想"。""",

    # Lesson 18
    "18a_itoa": """\
实现 itoa: 把整数按指定进制转换为字符串。

和第 9 课类似，但这次是通用版——支持任意进制 (2-16)。

步骤:
1. 取 num % base 得到一位
2. 查表 "0123456789ABCDEF" 得到对应字符
3. num /= base 继续下一位
4. 存入数组后逆序

用 do-while 确保 num=0 时也输出 "0"。

参考: 同目录下的原课 myprintf.c 中的 itoa。""",

    "18b_myprintf": """\
实现你自己的 printf! 这是一个精简版。

核心: 遍历格式字符串中的每个字符。
- 普通字符 → 直接输出
- 遇到 % → 看下一个字符决定怎么处理:
  - d → 用 itoa 把整数转成字符串打印
  - x → 用 itoa 以 16 进制打印
  - s → 直接打印字符串
  - c → 打印单个字符

需要用到 <stdarg.h> 中的 va_start, va_arg, va_end
来处理可变数量的参数。

参考: 同目录下的原课 myprintf.c。""",

    "18c_myprintf": """\
综合题: 同时实现 itoa 和 myprintf。

这是把 18a 和 18b 组合在一起的完整实现。
如果你已经完成了前两题，这里只需要把代码合并。

需要支持的格式符: %d, %x, %s, %c。

参考: 同目录下的原课 myprintf.c。""",

    # Lesson 19
    "19a_shell_parse": """\
实现命令行解析器——把一行命令拆成 argc/argv。

例如: "gcc -Wall main.c" → argv[0]="gcc", argv[1]="-Wall", argv[2]="main.c"

状态机思路:
- 状态 0 (在空格中): 跳过空格
- 状态 1 (在单词中): 记录字符

当从状态 0 进入状态 1 时——找到了一个新参数的起点!
在空格处放上 '\\0' 截断字符串。

参考: 同目录下的原课 shell.c 中的 shell_parse()。""",

    "19b_function_ptr": """\
用函数指针实现命令分发。

函数指针: 一个变量，指向一个函数而不是数据。

思路:
1. 定义函数指针 int (*pf)(int, char**)
2. 根据 argv[0] (命令名) 决定 pf 指向哪个函数
3. 用 strcmp 比较字符串
4. 通过 pf(argc, argv) 调用

参考: 同目录下的原课 shell.c。""",

    "19c_cmd_table": """\
用结构体数组实现命令表——更优雅的命令分发。

定义一个 struct command { 名称, 函数指针 } 的数组，
然后循环遍历这个表，用 strcmp 匹配命令名。

这比一长串 if-else 更易于扩展——
添加新命令只需在数组中加一行。

参考: 同目录下的原课 shell.c 课后练习。""",

    "19d_shell": """\
综合题: 解析命令行 + 查表分发。

把前面学的组合起来:
1. shell_parse(): 把输入拆成 argc/argv (来自 19a)
2. command_do(): 遍历命令表匹配并调用 (来自 19c)

如果你已经完成了前面的子题，这里就是把它们连接起来。

参考: 同目录下的原课 shell.c。""",

    # Lesson 20
    "20a_delcomment": """\
用状态机去除 C 代码中的注释。

C 有两种注释:
- /* ... */ 块注释 (可跨行)
- // ... 行注释 (到行尾)

状态机的关键状态:
- 正常: 输出字符
- 遇到 /: 下一个字符决定是否进入注释
- 在块注释中: 等待 */
- 在行注释中: 等待换行

逐字符读入，根据当前状态决定是输出还是跳过。

参考: 同目录下的原课 delcomment.c。""",

    "20b_getword": """\
实现一个词法切分函数 getword()。

getword 每次调用返回一个"词":
- 遇到 EOF → 返回空串
- 遇到非字母字符 → 返回该单字符
- 遇到字母 → 读取整个单词返回

关键: 当你发现一个字母时，要继续读直到遇到非字母，
但最后那个非字母字符不能丢! 用 ungetc 把它"退回去"。

参考: 同目录下的原课 expdefine.c 中的 getword。""",

    "20c_expdefine": """\
用表驱动状态机实现宏展开。

这道题要实现 7 个动作函数 (act_*)。
核心思路: 根据当前状态和输入类型查表，
得到要执行的动作和下一个状态。

关键动作:
- print_word: 如果词在宏表中则替换输出，否则原样输出
- save_to_buf/save_word: 暂存宏定义的内容
- get_macro_name/value: 记录 #define 的名称和值

这道题较难——建议先阅读原课代码理解整体框架。

参考: 同目录下的原课 expdefine.c。""",

    # Lesson 21
    "21_lexical_analyzer": """\
实现一个 DFA 驱动的词法分析器。

DFA (确定有限自动机) 的美妙之处:
代码逻辑不变，只需改变转移表就能识别不同的语言!

算法:
1. 逐字符读入，存入 buf
2. 查表 yy_nxt[当前状态][字符] 得到下一个状态
3. 如果状态变为负数——说明识别到了一个 token
4. 用 yy_accept 查出 token 类型并打印

这就是"机制与策略分离"的思想——
DFA 引擎是机制，转移表是策略。

参考: 本课 README 中的 DFA 详解。""",

    # Lesson 22
    "22a_generate": """\
生成 4 个不重复的随机数字 (0-9)。

关键: 怎么保证不重复？用一个 used[10] 数组标记!

步骤:
1. 生成一个随机数 d = my_rand() % 10
2. 检查 used[d] 是否已被使用
3. 如果已使用 → 重新生成 (do-while 很适合!)
4. 如果未使用 → 记录 secret[i] = d, 标记 used[d] = 1

do-while 的好处: 先执行后判断，确保至少生成一次。

参考: 同目录下的原课 guess.c 中的 generate_secret()。""",

    "22b_check": """\
计算猜测结果: ?A?B。

A = 数字和位置都对的个数
B = 数字对但位置不对的个数

算法:
- 外层循环 i (0-3): 比较每一位
  - guess[i] == secret[i] → A++
  - 否则用内层循环 j 在 secret 中找相同数字 → B++

注意: 只有位置不同时才算 B，位置相同的已经算 A 了。

参考: 同目录下的原课 guess.c 中的 check()。""",

    "22c_game_loop": """\
综合题: 实现完整的猜数游戏循环。

你需要实现:
1. parse_guess(): 把输入字符串转为数字数组
   - 检查长度 >= 4
   - 每个字符减 '0' 转为数字
2. main 中的游戏循环:
   - fgets 读取猜测
   - parse_guess 解析
   - check 比较
   - 打印 ?A?B
   - 如果 4A0B → 恭喜猜对!

注意最后要打印 "The secret was: ..." (无论是否猜对)。

参考: 同目录下的原课 guess.c。""",

    # Lesson 23
    "23a_print_board": """\
初始化并打印五子棋棋盘。

棋盘是一个二维数组。
- init_board(): 用 memset 把所有格子设为 0 (空)
- print_board(): 二重 for 循环打印

这是 compile 模式——编译通过即可，不检查输出。

参考: 同目录下的原课 gomoku.c。""",

    "23b_check_win": """\
判断五子棋是否有人获胜——需要检查四个方向的五连。

核心函数 check_dir(board, r, c, dr, dc, player):
从 (r,c) 沿 (dr,dc) 方向检查是否有连续 5 个同色棋子。

四个方向: 水平(0,1), 垂直(1,0), 右下对角(1,1), 左下对角(1,-1)。

check_win: 遍历棋盘每个位置，对每个方向调用 check_dir。

参考: 同目录下的原课 gomoku.c。""",

    "23c_gomoku": """\
实现五子棋完整游戏循环。

主循环结构:
1. turn 变量记录轮到谁 (turn % 2 区分黑白)
2. scanf 读取落子位置
3. 检查位置是否合法 (在棋盘内且为空)
4. 放置棋子
5. check_win 判断是否获胜
6. 如果棋盘满了也结束

别忘了处理棋盘满但无人获胜的平局情况。

参考: 同目录下的原课 gomoku.c。""",

    # Lesson 24
    "24a_find_istr": """\
实现大小写不敏感的字符串查找。

在 text 中查找 pattern，忽略大小写。

思路: 逐个位置尝试匹配。在每个位置:
- 用 tolower() 把两边的字符都转成小写再比较
- 如果所有字符都匹配 → 找到了，返回该位置
- 如果有一个不匹配 → 继续下一个位置

需要 <ctype.h> 中的 tolower 函数。

参考: 同目录下的原课 search.c 中的 find_istr()。""",

    "24b_extract_links": """\
从 HTML 中提取链接——解析 href="..." 属性。

算法:
1. 用 find_istr 在文本中查找 "href="
2. 找到后跳过 "href=" 这 5 个字符
3. 检查是否有引号 (单引号或双引号)
4. 读取直到找到闭合引号
5. 打印提取到的 URL
6. 继续查找下一个

这是字符串解析的经典模式: 查找标记 → 提取内容 → 继续。

参考: 同目录下的原课 search.c 中的 extract_links。""",

    "24c_search": """\
综合题: 完整的 HTML 链接提取器。

把前面的函数组合起来:
1. find_istr(): 大小写不敏感查找 (来自 24a)
2. extract_links(): 提取所有链接 (来自 24b)
3. main(): 读取全部输入 → 调用 extract_links → 打印统计

需要用 malloc 动态分配内存来存储输入文本，
读完后记得 free 释放。

参考: 同目录下的原课 search.c。""",
}


def update_toml_hints(toml_path: Path) -> int:
    """Update hints in a TOML file using line-by-line replacement. Returns count."""
    lines = toml_path.read_text(encoding="utf-8").splitlines(keepends=True)
    updated = 0
    current_name = None
    i = 0
    while i < len(lines):
        line = lines[i]
        # Track current exercise name
        m = re.match(r'^name\s*=\s*"([^"]+)"', line)
        if m:
            current_name = m.group(1)
        # Find hint line for a known exercise
        if current_name in NEW_HINTS and re.match(r"^hint\s*=\s*'''", line):
            # Find the end of the hint (closing ''')
            start = i
            if line.rstrip().endswith("'''") and line.count("'''") >= 2:
                end = i  # single line hint
            else:
                end = i + 1
                while end < len(lines) and "'''" not in lines[end]:
                    end += 1
            # Replace lines[start:end+1] with new hint
            new_hint = NEW_HINTS[current_name].rstrip()
            replacement = f"hint = '''\n{new_hint}\n'''\n"
            lines[start:end+1] = [replacement]
            updated += 1
            current_name = None  # don't match again
        i += 1

    if updated > 0:
        toml_path.write_text("".join(lines), encoding="utf-8")

    return updated


def main():
    total = 0
    for toml_path in sorted(EXERCISES_DIR.rglob("exercises.toml")):
        count = update_toml_hints(toml_path)
        if count > 0:
            rel = toml_path.relative_to(EXERCISES_DIR.parent)
            print(f"  [{count:2d}] {rel}")
            total += count
    print(f"\nUpdated {total} hints")


if __name__ == "__main__":
    main()
