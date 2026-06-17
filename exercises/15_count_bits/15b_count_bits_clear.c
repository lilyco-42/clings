/*
 * Lesson 15b: n&(n-1) 清最低位 — 统计 bit 1 个数
 *
 * 知识点：n & (n-1) 清除最右边的 1, while 循环
 * 参考原课 bits.c 方法二
 *
 * 任务：实现 count_bits(num) 用 n&(n-1) 技巧:
 *       while (num != 0) { num = num & (num - 1); sum++; }
 *       每次循环清除 num 最右边的一个 1
 *
 * 原理：0x64 = 0110 0100, 0x63 = 0110 0011
 *        0x64 & 0x63 = 0110 0000 → 清掉了最右的 1
 *
 * 验证：输入 15 → "4\n"; 输入 1024 → "1\n"
 */

#include <stdio.h>

int count_bits(int num)
{
	int sum = 0;

#error TODO: while (num != 0) { num &= (num - 1); sum++; }
	return sum;
}

int main(void)
{
	int num;

	scanf("%d", &num);
	printf("%d\n", count_bits(num));

	return 0;
}
