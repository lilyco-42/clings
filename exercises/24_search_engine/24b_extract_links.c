/*
 * Lesson 24b: 从 HTML 中提取链接
 *
 * 知识点：字符串解析，引号处理，状态跟踪，指针操作
 * 参考原课 search.c 中的 extract_links() 函数
 *
 * 任务：find_istr 已实现，学生实现 extract_links(html):
 *       1. while 循环用 find_istr 查找 "href="
 *       2. 跳过 "href=" (p += 5), 跳过空格
 *       3. 检测引号 ('"' 或 '\''), 提取 URL 直到引号结束或空格/换行
 *       4. printf("[%d] %s\n", ++count, url)
 *       5. 返回找到的链接数
 *
 * 验证："<a href=\"http://a.com\"> <a href='http://b.com'>"
 *       → "[1] http://a.com\n[2] http://b.com\n"
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_URL 1024

static const char *find_istr(const char *text, const char *pattern)
{
	int plen = strlen(pattern);
	while (*text) {
		int match = 1;
		for (int i = 0; i < plen && text[i]; i++) {
			if (tolower(text[i]) != tolower(pattern[i])) {
				match = 0;
				break;
			}
		}
		if (match) return text;
		text++;
	}
	return NULL;
}

static int extract_links(const char *html)
{
#error TODO: Extract URLs from href= attributes in HTML. Run "clings hint" for help.
}

int main(void)
{
	char html[4096];
	int total = 0;

	/* 读取全部 stdin 到 html */
	int len = 0;
	int n;
	while ((n = fread(html + len, 1, sizeof(html) - len - 1, stdin)) > 0)
		len += n;
	html[len] = '\0';

	total = extract_links(html);
	printf("\nTotal: %d links found.\n", total);

	return 0;
}
