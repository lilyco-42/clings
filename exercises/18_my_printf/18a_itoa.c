/*
 * Lesson 18a: itoa — 整数转任意进制字符串
 *
 * 知识点：do-while, 取余 %, 字符映射 "0123456789ABCDEF", 字符串逆序
 * 参考原课 myprintf.c 中的 itoa 函数
 *
 * 任务：实现 itoa(num, buf, base):
 *       1. do-while: rest = num % base; buf[i++] = hex[rest]; num /= base;
 *       2. buf[i] = '\0'
 *       3. 逆序：for (j = 0; j < i/2; j++) 交换 buf[j] 和 buf[i-1-j]
 *
 * 验证：输入 "100 10" → "100\n"; 输入 "255 16" → "FF\n"
 */

#include <stdio.h>

void itoa(int num, char *buf, int base)
{
	char *hex = "0123456789ABCDEF";

#error TODO: Fix this exercise. Run "clings hint" for help.
}

int main(void)
{
	int num, base;
	char buf[64];

	scanf("%d %d", &num, &base);

	itoa(num, buf, base);
	printf("%s\n", buf);

	return 0;
}
