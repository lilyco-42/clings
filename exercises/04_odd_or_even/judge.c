/*
 * Lesson 04: 判断奇偶
 *
 * 知识点：条件语句 if/else, 格式化输入 scanf, 关系运算符 ==,
 *         取地址符 &, 取模运算符 %
 *
 * 任务：参考原课代码，完成完整程序:
 *       1. 用 scanf 读取一个整数 num (注意 & 取地址)
 *       2. 用 if/else 和 % 判断奇偶
 *       3. 偶数打印 "num %d is even\n"
 *       4. 奇数打印 "num %d is odd\n"
 *
 * 课堂讨论：scanf 的第 2 个参数如果不用 &，会有什么后果？
 *           想一想为什么 printf 不需要 &num，而 scanf 需要 &
 *
 * 验证：输入 7 → "num 7 is odd\n"; 输入 12 → "num 12 is even\n"
 */

#include <stdio.h>

int main(void)
{
	int num;

	scanf("%d", &num);

#error TODO: Use if/else to check num%2==0, print "num %d is even/odd\n". Run "clings hint" for help.

	return 0;
}
