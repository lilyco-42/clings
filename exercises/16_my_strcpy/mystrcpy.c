/*
 * Lesson 16: 字符串拷贝
 *
 * 知识点：指针，const, assert, while (*dest++ = *src++) 惯用法
 *
 * 任务：参考原课 mystrcpy.c，实现 mystrcpy 函数:
 *       1. char *mystrcpy(char *dest, const char *src)
 *       2. 逐字符复制直到 '\0' 也复制过去
 *       3. 返回 dest 起始地址
 *       4. main 中从 stdin 读入字符串，用 mystrcpy 复制后打印
 *
 * 验证：输入 "copy me" → "copy me\n"
 */

#include <stdio.h>
#include <assert.h>

char *mystrcpy(char *dest, const char *src)
{
	assert(dest != NULL && src != NULL);

#error TODO: Implement mystrcpy using pointer idiom. Run "clings hint" for help.
}

int main(void)
{
	char s1[256] = "";
	char s2[256];

	fgets(s2, sizeof(s2), stdin);
	/* 去掉换行 */
	int i = 0;
	while (s2[i] && s2[i] != '\n') i++;
	s2[i] = '\0';

	mystrcpy(s1, s2);
	printf("%s\n", s1);

	return 0;
}
