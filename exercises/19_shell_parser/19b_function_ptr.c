/*
 * Lesson 19b: 函数指针
 *
 * 知识点：函数指针 int (*pf)(int, int), 全局函数指针，strcmp 命令分发
 * 参考原课 shell.c 中的全局 pf + math_main 设计
 *
 * 任务:
 *       1. 实现 add(a, b) 和 sub(a, b): 返回 a+b 和 a-b
 *       2. main 中用 strcmp 匹配 argv[0]，设置全局 pf 和 opchar
 *       3. 调用 math_main(argc, argv) 完成计算和输出
 *
 * 验证："add 10 3" → "result: 10 + 3 = 13\n"; "sub 10 3" → "result: 10 - 3 = 7\n"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int (*pf)(int, int);
char opchar;

int shell_parse(char *buf, char *argv[])
{
	int argc = 0;
	int state = 0;

	while (*buf && *buf != '\n')
	{
		if (*buf != ' ' && state == 0) { argv[argc++] = buf; state = 1; }
		if (*buf == ' ' && state == 1) { *buf = '\0'; state = 0; }
		buf++;
	}
	if (*buf == '\n') *buf = '\0';
	return argc;
}

int math_main(int argc, char *argv[])
{
	int a, b;
	int result;

	if (argc < 3)
		return -1;

	a = atoi(argv[1]);
	b = atoi(argv[2]);

	result = pf(a, b);

	printf("result: %s %c %s = %d\n", argv[1], opchar, argv[2], result);

	return 0;
}

#error TODO: Fix this exercise. Run "clings hint" for help.

int main(void)
{
	char buf[256];
	int argc;
	char *argv[10];

	fgets(buf, sizeof(buf), stdin);
	argc = shell_parse(buf, argv);

#error TODO: Fix this exercise. Run "clings hint" for help.

	return 0;
}
