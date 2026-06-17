/*
 * Lesson 20: 预处理器实现
 *
 * 知识点：状态机，宏替换，字符串处理，debug 调试宏
 *
 * 任务：参考原课 expdefine.c，实现 #define 宏替换:
 *       1. 读入一行，如果是 #define NAME VALUE，存储宏
 *       2. 否则将行中的 NAME 替换为 VALUE 后输出
 *
 * 验证：输入 "#define N 10\nint x = N;\n" → "int x = 10;\n"
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct macro
{
	char name[64];
	char value[64];
} macros[16];

int macro_count = 0;

void replace_and_print(char *line)
{
#error TODO: Replace macro names with values in the input line. Run "clings hint" for help.
}

int main(void)
{
	char line[4096];

	while (fgets(line, sizeof(line), stdin))
	{
		if (strncmp(line, "#define ", 8) == 0)
		{
			char *p = line + 8;
			while (*p == ' ') p++;
			int i = 0;
			while (*p && !isspace((unsigned char)*p))
				macros[macro_count].name[i++] = *p++;
			macros[macro_count].name[i] = '\0';
			while (*p == ' ' || *p == '\t') p++;
			i = 0;
			while (*p && *p != '\n')
				macros[macro_count].value[i++] = *p++;
			macros[macro_count].value[i] = '\0';
			macro_count++;
		}
		else
		{
			replace_and_print(line);
		}
	}

	return 0;
}
