/*
 * Lesson 18: 实现 printf — itoa + myprintf 综合
 *
 * 知识点：do-while, 字符映射，字符串逆序，stdarg.h, va_list 可变参数
 * 参考原课 myprintf.c
 *
 * 任务:
 *       1. 实现 itoa(num, buf, base): do-while 取余 + 逆序 (参考 18a)
 *       2. 实现 myprintf(format, ...): va_list 遍历 + %d/%x/%s/%c (参考 18b)
 *
 * 课堂讨论：va_arg 是如何知道下一个参数的地址的？
 *
 * 验证："a = 100, b = 0xC8\nc = A, s = helloworld\n"
 */

#include <stdio.h>
#include <stdarg.h>

void itoa(int num, char *buf, int base)
{
	char *hex = "0123456789ABCDEF";

#error TODO: Implement itoa (same as 18a). Run "clings hint" for help.
}

void myputs(char *buf)
{
	while (*buf)
		putchar(*buf++);
}

int myprintf(const char *format, ...)
{
#error TODO: Implement myprintf with va_list (same as 18b). Run "clings hint" for help.
	return 0;
}

int main(void)
{
	myprintf("a = %d, b = 0x%x\n", 100, 200);
	myprintf("c = %c, s = %s\n", 'A', "helloworld");

	return 0;
}
