/*
 * Lesson 19d: 命令解释器综合
 *
 * 知识点：状态机解析，函数指针，结构体命令表，综合运用
 * 参考原课 shell.c — 从字符串解析到命令执行的完整流程
 *
 * 任务：同时实现 shell_parse() 和 command_do():
 *       1. shell_parse(buf, argv): 状态机切分命令行 (参考 19a)
 *       2. command_do(argc, argv): 遍历 op[] 匹配 argv[0],
 *          设置全局 pf/opchar, 调用 math_main (参考 19c)
 *
 * 验证："add 100 200\n" → "result: 100 + 200 = 300\n"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int (*pf)(int, int);
char opchar;

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

struct operation
{
	char name[8];
	int (*pf)(int, int);
	char opchar;
} op[] =
{
	{ "add", add, '+' },
	{ "sub", sub, '-' },
	{ "mul", mul, 'x' },
	{ "div", mydiv, '/' },
	{ "p", power, '^' }
};

int shell_parse(char *buf, char *argv[])
{
#error TODO: Fix this exercise. Run "clings hint" for help.
	return 0;
}

int command_do(int argc, char *argv[])
{
#error TODO: Fix this exercise. Run "clings hint" for help.
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
