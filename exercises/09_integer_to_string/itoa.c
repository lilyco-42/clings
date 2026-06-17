/*
 * Lesson 09: 整型转字符串
 *
 * 知识点：数组 array, 字符串逆序 reverse, 进制表示，do-while
 * 参考原课 itoa.c — 全部在 main 中完成，不抽函数
 *
 * 任务：在 main 中实现整型转字符串:
 *       1. do-while: buf[i] = num % 10 + '0'; i++; num /= 10;
 *       2. buf[i] = '\0' 终止
 *       3. 逆序：for j=0..i/2, 用 tmp 交换 buf[j] 和 buf[i-1-j]
 *
 * 课堂讨论：do-while 的循环可以用 while 替换吗？为什么？
 *           要交换两个字节的内容，tmp 变量是必须的吗？
 *
 * 验证：输入 123 → "buf = 123\n"; 输入 9876 → "buf = 9876\n"
 */

#include <stdio.h>

int main(void)
{
	int num;
	char buf[10];
	int i = 0;
	int j = 0;

	scanf("%d", &num);

#error TODO: Implement itoa: extract digits with do-while, then reverse. Run "clings hint" for help.

	printf("buf = %s\n", buf);

	return 0;
}
