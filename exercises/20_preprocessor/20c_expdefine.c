/*
 * Lesson 20c: 表驱动状态机 — 宏展开
 *
 * 知识点：二维数组，函数指针，表驱动状态机设计
 * 参考原课 expdefine.c — 用 state_transition[][] 和 act_table[][] 驱动
 *
 * 任务：实现 7 个 action 函数:
 *       1. act_print_word(): 检查是否是宏名，是则输出宏值，否则原样输出
 *       2. act_save_to_buf(): 将当前 word 追加到 buf
 *       3. act_print_buf_and_word(): 输出 buf 和 word，清空 buf
 *       4. act_save_word(): 将 word 追加到 word_buf 和 buf
 *       5. act_get_macro_name(): 保存 word_buf 为宏名，清空 word_buf
 *       6. act_get_macro_value(): 保存 word_buf 为宏值，macro_counter++
 *       7. act_null(): 空操作
 *
 * 验证："#define N 10\nint x = N;\n" → "int x = 10;\n"
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

int get_input_type(char *word)
{
	if (strcmp(word, " ") == 0 || strcmp(word, "\t") == 0)
		return 0;
	if (strcmp(word, "#") == 0)
		return 1;
	if (strcmp(word, "define") == 0)
		return 2;
	if (strcmp(word, "\n") == 0)
		return 4;
	return 3;
}

char word[64];
char word_buf[64];
char buf[128];

struct macro
{
	char name[64];
	char value[64];
} macros[16];

int macro_counter = 0;

void act_print_word(void)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

void act_save_to_buf(void)
{
#error TODO: strcat(buf, word)
}

void act_print_buf_and_word(void)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

void act_save_word(void)
{
#error TODO: strcat(word_buf, word); strcat(buf, word)
}

void act_get_macro_name(void)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

void act_get_macro_value(void)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

void act_null(void) {}

enum { s0 = 0, s1, s2, s3, s4, s5, s6, s7 };

int state_transition[8][5] =
{
	{ s0, s1, s7, s7, s0 },
	{ s1, s0, s2, s0, s0 },
	{ s3, s0, s0, s0, s0 },
	{ s3, s4, s4, s4, s0 },
	{ s5, s5, s4, s0, s0 },
	{ s5, s6, s6, s6, s0 },
	{ s6, s6, s6, s6, s0 },
	{ s7, s7, s7, s7, s0 },
};

typedef void (*PF)(void);
PF act_table[8][5] =
{
	{ act_print_word, act_save_to_buf, act_print_word, act_print_word, act_print_word },
	{ act_save_to_buf, act_print_buf_and_word, act_save_to_buf, act_print_buf_and_word, act_print_buf_and_word },
	{ act_save_to_buf, act_print_buf_and_word, act_print_buf_and_word, act_print_buf_and_word, act_print_buf_and_word },
	{ act_save_to_buf, act_save_word, act_save_word, act_save_word, act_print_buf_and_word },
	{ act_get_macro_name, act_get_macro_name, act_save_word, act_print_buf_and_word, act_get_macro_value },
	{ act_null, act_save_word, act_save_word, act_save_word, act_get_macro_value },
	{ act_save_word, act_save_word, act_save_word, act_save_word, act_get_macro_value },
	{ act_print_word, act_print_word, act_print_word, act_print_word, act_print_word },
};

void getword(char *word)
{
	char c;

	c = getchar();

	if (c == EOF)
	{
		*word = '\0';
		return;
	}

	if (!isalpha(c))
	{
		*word++ = c;
		*word = '\0';
		return;
	}

	do
	{
		*word++ = c;
		c = getchar();
	} while (isalnum(c) || c == '_');

	ungetc(c, stdin);
	*word = '\0';
}

int main(void)
{
	int state = 0;

	while (1)
	{
		int input;
		void (*pf)(void);

		getword(word);
		if (strcmp(word, "") == 0)
			break;

		input = get_input_type(word);
		pf = act_table[state][input];
		pf();
		state = state_transition[state][input];
	}

	return 0;
}
