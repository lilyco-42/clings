/*
 * Lesson 15a: 逐位检测 — 统计 bit 1 个数
 *
 * 知识点：位运算 &, 位移 <<, 掩码 1<<i, for 循环
 * 参考原课 bits.c 方法一
 *
 * 任务：实现 count_bits(num) 用逐位检测:
 *       for (i = 0; i < 32; i++)
 *           if (num & (1 << i)) sum++;
 *
 * 验证：输入 15 → "4\n"; 输入 1024 → "1\n"
 */

#include <stdio.h>

int count_bits(int num)
{
	int sum = 0;
	int i;

#error TODO: Fix this exercise. Run "clings hint" for help.
	return sum;
}

int main(void)
{
	int num;

	scanf("%d", &num);
	printf("%d\n", count_bits(num));

	return 0;
}
