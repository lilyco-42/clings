/*
 * Lesson 13a: 函数封装 — get_last_char
 *
 * 知识点：字符串遍历，函数设计，while 循环
 * 参考原课 restrict.c 中的 get_last_char() 函数
 *
 * 任务：实现 get_last_char(str) 函数:
 *       1. 定义 char c, int i = 0
 *       2. while (str[i]) { c = str[i]; i++; }
 *       3. return c — 返回字符串最后一个字符
 *
 * 验证：输入 "A23456" → "6\n"; 输入 "hello" → "o\n"
 */

#include <stdio.h>

char get_last_char(char str[])
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

int main(void)
{
	char buf[64];

	scanf("%s", buf);
	printf("%c\n", get_last_char(buf));

	return 0;
}
