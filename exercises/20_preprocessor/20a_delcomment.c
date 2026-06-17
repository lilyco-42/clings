/*
 * Lesson 20a: 去注释状态机
 *
 * 知识点：状态机，字符分类函数，if-else 状态转换
 * 参考原课 delcomment.c — 用 get_input_type() 分类字符
 *
 * 任务：实现 7 状态的去注释状态机:
 *       state 0: 正常代码
 *       state 1: 遇到 '/'，待定 → /* 或 // 或普通 /
 *       state 2: 进入块注释 /* ...
 *       state 3: 块注释中遇到 *，可能结束
 *       state 4: 进入行注释 // ...
 *       state 5: 字符常量中 '...'
 *       state 6: 字符常量中遇到 \ 转义
 *
 *       根据 state 和 input (get_input_type 返回值) 做状态转换:
 *       - state 0: 正常代码，遇 input==1(/) → state 1, 遇 input==4(') → state 5
 *       - state 1: 遇 input==2(*) → state 2, 遇 input==1(/) → state 4, 否则输出 / 和 c
 *       - state 2: 遇 input==2(*) → state 3, 其余留在 state 2
 *       - state 3: 遇 input==1(/) → state 0 (注释结束), 遇 input==2(*) 留 3, 其余回 2
 *       - state 4: 遇 input==3(\n) → state 0 (行注释结束，输出\n)
 *       - state 0/5: putchar(c)
 *
 * 验证："int x; / * comment * /\nint y;\n" → "int x; \nint y;\n"
 */

#include <stdio.h>

int get_input_type(char c)
{
	if (c == '/')
		return 1;
	if (c == '*')
		return 2;
	if (c == '\n')
		return 3;
	if (c == '\'')
		return 4;
	if (c == '\\')
		return 5;
	return 0;
}

int main(void)
{
	int state = 0;

	while (1)
	{
		char c;
		int input;

		c = getchar();
		if (c == EOF)
			break;

		input = get_input_type(c);

#error TODO: Fix this exercise. Run "clings hint" for help.
	}

	return 0;
}
