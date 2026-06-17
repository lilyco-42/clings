/*
 * Lesson 09b: 整型转十六进制字符串
 *
 * 知识点：16 进制转换，字符映射表 "0123456789ABCDEF", do-while, 逆序
 *
 * 任务：在 09 itoa 基础上，扩展支持 base=16:
 *       1. char *itoa_hex(int num, char *buf)
 *       2. do-while: rest = num % 16; buf[i++] = hex[rest]; num /= 16;
 *       3. buf[i] = '\0'
 *       4. 逆序：交换 buf[j] 和 buf[i-1-j]
 *       5. main 中 scanf 读入整数，调用 itoa_hex 后打印
 *
 * 验证：输入 255 → "FF\n"; 输入 16 → "10\n"
 */

#include <stdio.h>

char *itoa_hex(int num, char *buf)
{
	char *hex = "0123456789ABCDEF";

#error TODO: Fix this exercise. Run "clings hint" for help.
	return buf;
}

int main(void)
{
	int num;
	char buf[64];

	scanf("%d", &num);

	itoa_hex(num, buf);
	printf("%s\n", buf);

	return 0;
}
