/*
 * Lesson 24a: 大小写不敏感子串查找
 *
 * 知识点：指针操作，tolower, 字符串匹配
 * 参考原课 search.c 中的 find_istr() 函数
 *
 * 任务：实现 find_istr(text, pattern):
 *       1. 计算 plen = strlen(pattern)
 *       2. while (*text) 逐位置尝试匹配
 *       3. 内层 for 循环比较 tolower(text[i]) vs tolower(pattern[i])
 *       4. 全部匹配返回 text 指针，否则 text++ 继续
 *
 * 验证：text="Hello World" pattern="world" → "found\n"
 *       text="abc" pattern="xyz" → "not found\n"
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

const char *find_istr(const char *text, const char *pattern)
{
#error TODO: Case-insensitive string search using tolower comparison. Run "clings hint" for help.
}

int main(void)
{
	char text[1024];
	char pattern[256];

	fgets(text, sizeof(text), stdin);
	text[strcspn(text, "\n")] = '\0';

	fgets(pattern, sizeof(pattern), stdin);
	pattern[strcspn(pattern, "\n")] = '\0';

	if (find_istr(text, pattern))
		printf("found\n");
	else
		printf("not found\n");

	return 0;
}
