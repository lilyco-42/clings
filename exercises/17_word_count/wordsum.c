/*
 * Lesson 17: 统计单词
 *
 * 知识点：状态机编程，get_input_type 函数，指针 p 记录单词起始，counter 计长度
 * 参考原课 wordsum.c — 4 个状态转换分支 + 打印每个单词
 *
 * 任务：实现状态机主循环:
 *       1. state==0 && input==0: 保持 state=0 (连续空白)
 *       2. state==0 && input==1: state=1, p=&buf[i], counter=0, counter++
 *       3. state==1 && input==0: state=0, words++, 打印 "word %d found!\n"
 *          然后 for 循环打印 p[0..counter-1] 每个字符，再打印 "\n"
 *       4. state==1 && input==1: state=1, counter++
 *       5. 遇到 '\0' 跳出循环
 *       6. 最后打印 "there is %d words found!\n"
 *
 * 验证："This is a book" → "word 1 found!\nThis\n...there is 4 words found!\n"
 */

#include <stdio.h>

int get_input_type(char c)
{
	if (c >= 'a' && c <= 'z')
		return 1;

	if (c >= 'A' && c <= 'Z')
		return 1;

	return 0;
}

int main(void)
{
	char buf[512];
	int state = 0;
	int i = 0;
	int words = 0;
	char *p = NULL;
	int counter = 0;

	fgets(buf, sizeof(buf), stdin);
	/* 去掉换行 */
	for (i = 0; buf[i]; i++)
		if (buf[i] == '\n') { buf[i] = '\0'; break; }

	i = 0;

#error TODO: Implement state machine loop with 4 branches for word counting. Run "clings hint" for help.

	printf("there is %d words found!\n", words);
	return 0;
}
