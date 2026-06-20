#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINES 65536

static char *pv[MAX_LINES];
static int lines = 0;

static int opt_u = 0;  // -u 去重
static int opt_r = 0;  // -r 逆序
static int opt_n = 0;  // -n 数值排序

static int cmp_str(const void *s1, const void *s2)
{
	const char *line1 = *(const char **)s1;
	const char *line2 = *(const char **)s2;
	return strcmp(line1, line2);
}

static int cmp_str_r(const void *s1, const void *s2)
{
	return -cmp_str(s1, s2);
}

static int cmp_str_n(const void *s1, const void *s2)
{
	int a = atoi(*(const char **)s1);
	int b = atoi(*(const char **)s2);
	return (a > b) - (a < b);  // 避免减法溢出
}

static int cmp_str_nr(const void *s1, const void *s2)
{
	return -cmp_str_n(s1, s2);  // 数值逆序
}

static void display(void)
{
	const char *prev = "";
	for (int i = 0; i < lines; i++) {
		if (opt_u && strcmp(pv[i], prev) == 0)
			continue;
		printf("%s", pv[i]);
		prev = pv[i];
	}
}

static void free_lines(void)
{
	for (int i = 0; i < lines; i++)
		free(pv[i]);
	lines = 0;
}

int sort_main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: sort <filename> [-u] [-r] [-n]\n");
		return 1;
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror(argv[1]);
		return 1;
	}

	char buf[1024];
	lines = 0;
	while (fgets(buf, sizeof(buf), fp) && lines < MAX_LINES) {
		pv[lines] = malloc(strlen(buf) + 1);
		if (pv[lines] == NULL)
			break;
		strcpy(pv[lines], buf);
		lines++;
	}
	fclose(fp);

	// 解析选项: 支持任意顺序和组合
	opt_u = opt_r = opt_n = 0;
	for (int i = 2; i < argc; i++) {
		if (argv[i][0] != '-') continue;
		for (const char *p = argv[i] + 1; *p; p++) {
			switch (*p) {
			case 'u': opt_u = 1; break;
			case 'r': opt_r = 1; break;
			case 'n': opt_n = 1; break;
			}
		}
	}

	// 选择比较函数并排序
	int (*cmp)(const void *, const void *) = cmp_str;
	if (opt_n && opt_r) cmp = cmp_str_nr;
	else if (opt_n)     cmp = cmp_str_n;
	else if (opt_r)     cmp = cmp_str_r;

	qsort(pv, lines, sizeof(char *), cmp);
	display();

	free_lines();
	return 0;
}
