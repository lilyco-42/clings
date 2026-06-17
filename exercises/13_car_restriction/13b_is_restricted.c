/*
 * Lesson 13b: switch + 三元表达式 — is_restricted
 *
 * 知识点：enum, switch-case, 三元表达式 ? :, 函数封装
 * 参考原课 restrict.c 中的 is_restricted() 函数
 *
 * 任务：实现 is_restricted(tail_num, today):
 *       用 switch(tail_num) 判断尾号是否在今日限行:
 *       - case 0/5: ret = (today == MONDAY) ? 1 : 0; break;
 *       - case 1/6: ret = (today == TUESDAY) ? 1 : 0; break;
 *       - case 2/7: ret = (today == WEDNESDAY) ? 1 : 0; break;
 *       - case 3/8: ret = (today == THURSDAY) ? 1 : 0; break;
 *       - case 4/9: ret = (today == FRIDAY) ? 1 : 0; break;
 *
 * 验证：输入 "2 6" → "restricted\n"; 输入 "3 9" → "free\n"
 */

#include <stdio.h>

enum day
{
	MONDAY = 1,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY
};

int is_restricted(int tail_num, enum day today)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

int main(void)
{
	int weekday, tail_num;

	scanf("%d %d", &weekday, &tail_num);

	if (is_restricted(tail_num, (enum day)weekday))
		printf("restricted\n");
	else
		printf("free\n");

	return 0;
}
