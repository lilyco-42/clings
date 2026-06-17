/*
 * Lesson 05b: continue 语句 — 只加偶数
 *
 * 知识点：continue 语句，if 条件跳过
 *         原课 sum.c 使用 if (i % 2 == 1) continue; 跳过奇数
 *
 * 任务：参考原课 sum.c，完成以下程序:
 *       1. for 循环 i 从 0 到 100
 *       2. 如果 i 是奇数 (i % 2 == 1)，用 continue 跳过
 *       3. 否则累加 sum += i
 *       4. 打印 "sum = %d\n" (结果应为 2550)
 *
 * 验证：stdout 精确匹配 "sum = 2550\n"
 */

#include <stdio.h>

int main(void)
{
	int i = 0;
	int sum = 0;

	for (i = 0; i <= 100; i++)
	{
#error TODO: Fix this exercise. Run "clings hint" for help.

	}

	printf("sum = %d\n", sum);

	return 0;
}
