/*
 * Lesson 16b: delta 优化版 strcpy
 *
 * 知识点：指针运算优化，delt = dst - src, 减少变量存取次数
 *
 * 任务：参考原课 README "算法优化 1"，实现 delta 优化版:
 *       1. char *mystrcpy_delt(char *dst, const char *src)
 *       2. char *s = (char *)src; int delt = dst - src;
 *       3. while (*s != '\0') { s[delt] = *s; s++; }
 *       4. s[delt] = '\0'; return dst;
 *
 * 出处：林锐《高质量程序设计指南》— 减少对 dst 指针变量的存取次数
 * 注意：dst 和 src 必须在同一数组内，delt 指针运算才合法
 *
 * 验证：输入 "delta copy" → "delta copy\n"
 */

#include <stdio.h>

char *mystrcpy_delt(char *dst, const char *src)
{
#error TODO: s = (char*)src; delt = dst - src; while (*s) { s[delt] = *s; s++; } s[delt] = '\0';
	return dst;
}

/* buf 保证 src 和 dst 在同一数组内，delt 指针运算合法 */
char buf[512];

int main(void)
{
	char *src = buf;
	char *dst = buf + 256;

	fgets(src, 256, stdin);
	int i = 0;
	while (src[i] && src[i] != '\n') i++;
	src[i] = '\0';

	mystrcpy_delt(dst, src);
	printf("%s\n", dst);

	return 0;
}
