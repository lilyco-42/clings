/*
 * Lesson 19: 命令解释器
 *
 * 知识点：argc/argv, 状态机解析，函数指针，结构体数组
 *
 * 任务：参考原课 shell.c 的状态机解析:
 *       1. 遍历 buf，用状态机切分：空格→state=0, 非空格→state=1
 *       2. state 0→1 时记录 argv[argc++] = &buf[i]
 *       3. state 1→0 时 buf[i] = '\0' 截断
 *       4. 用 | 连接所有 token 打印
 *
 * 验证：输入 "gcc -Wall main.c" → "gcc|-Wall|main.c\n"
 */

#include <stdio.h>

int main(void)
{
	char buf[256];
	char *argv[32];
	int argc = 0;
	int state = 0;
	int i;

	fgets(buf, sizeof(buf), stdin);

#error TODO: Fix this exercise. Run "clings hint" for help.

	/* 用 | 连接打印 */
	for (i = 0; i < argc; i++)
	{
		if (i > 0) printf("|");
		printf("%s", argv[i]);
	}
	printf("\n");

	return 0;
}
