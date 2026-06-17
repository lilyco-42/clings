/*
 * Lesson 20b: getword — 字符→单词切分
 *
 * 知识点：字符分类 isalpha/isalnum, ungetc 回退，字符串指针
 * 参考原课 expdefine.c 中的 getword 函数
 *
 * 任务：实现 getword(word) 从 stdin 读取一个"单词":
 *       1. c = getchar(); 若 EOF → word[0]='\0' 返回
 *       2. 若 c 不是字母 → word 只存这一个字符：*word++ = c; *word = '\0'
 *       3. 若 c 是字母 → do { *word++ = c; c = getchar(); } while (isalnum(c) || c == '_')
 *          循环结束后 ungetc(c, stdin) 回退非单词字符，*word = '\0'
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
	int i;
	for (i = 0; i < macro_counter; i++)
	{
		if (strcmp(word, macros[i].name) == 0)
		{
			printf("%s", macros[i].value);
			return;
		}
	}
	printf("%s", word);
}

void act_save_to_buf(void)
{
	strcat(buf, word);
}

void act_print_buf_and_word(void)
{
	printf("%s", buf);
	printf("%s", word);
	strcpy(buf, "");
}

void act_save_word(void)
{
	strcat(word_buf, word);
	strcat(buf, word);
}

void act_get_macro_name(void)
{
	strcpy(macros[macro_counter].name, word_buf);
	strcpy(word_buf, "");
}

void act_get_macro_value(void)
{
	strcpy(macros[macro_counter].value, word_buf);
	strcpy(word_buf, "");
	strcpy(buf, "");
	strcpy(word, "");
	macro_counter++;
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
#error TODO: Fix this exercise. Run "clings hint" for help.
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
