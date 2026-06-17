/*
 * Lesson 19c: 结构体命令表 — struct operation
 *
 * 知识点：结构体数组，函数指针，循环匹配，表驱动
 * 参考原课 shell.c 的 struct operation + command_do 设计
 *
 * 任务:
 *       1. 定义 struct operation { char name[8]; int (*pf)(int, int); char opchar; }
 *       2. 初始化 op[] 命令表：add/sub/mul/div/power 五个命令
 *       3. 实现 command_do(argc, argv): 遍历 op[] 匹配 argv[0],
 *          设置全局 pf 和 opchar, 调用 math_main
 *
 * 验证："mul 4 5" → "result: 4 x 5 = 20\n"
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

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int mydiv(int a, int b) { if (b != 0) return a / b; return 0; }

int power(int a, int b)
{
	int result = 1;
	int i;
	for (i = 0; i < b; i++)
		result *= a;
	return result;
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

#error TODO: Define struct operation and op[] array with 5 operations. Run "clings hint" for help.

int command_do(int argc, char *argv[])
{
#error TODO: Loop through op[] table, match command, call function. Run "clings hint" for help.
	return 0;
}

int main(void)
{
	char buf[256];
	int argc;
	char *argv[10];

	fgets(buf, sizeof(buf), stdin);
	argc = shell_parse(buf, argv);
	command_do(argc, argv);

	return 0;
}
