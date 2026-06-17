/*
 * Lesson 24c: 简单搜索引擎 — HTML 链接提取器
 *
 * 知识点：字符串匹配，指针操作，引号解析，动态内存
 * 参考原课 search.c — 从 HTML 中提取所有 <a href="..."> 链接
 *
 * 任务：read_all 已实现 (读取全部 stdin)，学生实现:
 *       1. find_istr(text, pattern): tolower 大小写不敏感匹配
 *       2. extract_links(html): 循环 find_istr("href=") → 解析引号 → 提取 URL
 *       3. main(): 调用 read_all + extract_links + 打印总计
 *
 * 验证：HTML 含 2 个 href → "[1] url1\n[2] url2\n\nTotal: 2 links found.\n"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_URL 1024

static const char *find_istr(const char *text, const char *pattern)
{
#error TODO: Case-insensitive string search using tolower comparison. Run "clings hint" for help.
}

static int extract_links(const char *html)
{
#error TODO: Extract URLs from href= attributes in HTML. Run "clings hint" for help.
}

/* 读取全部 stdin 到动态 buffer (给出，不需要实现) */
static char *read_all(FILE *fp)
{
	size_t cap = 4096, len = 0;
	char *buf = malloc(cap);
	if (!buf) return NULL;

	size_t n;
	while ((n = fread(buf + len, 1, cap - len - 1, fp)) > 0) {
		len += n;
		if (len + 1 >= cap) {
			cap *= 2;
			buf = realloc(buf, cap);
			if (!buf) return NULL;
		}
	}
	buf[len] = '\0';
	return buf;
}

int main(void)
{
#error TODO: Read all input, extract links, print total, free memory. Run "clings hint" for help.
	return 0;
}
