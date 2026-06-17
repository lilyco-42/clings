/*
 * Lesson 15: popcount 分治法 — 统计 bit 1 个数
 *
 * 知识点：位运算分治，并行加法，掩码常数 M1-M5
 * 参考原课 bits.c 方法三 — popcount
 *
 * 任务：用 5 层掩码 - 移位 - 加法，O(1) 统计 bit 1 个数
 *       M1=0x55555555  — 每 2 位一组，相邻位相加
 *       M2=0x33333333  — 每 4 位一组
 *       M3=0x0F0F0F0F  — 每 8 位一组
 *       M4=0x00FF00FF  — 每 16 位一组
 *       M5=0x0000FFFF  — 高 16 位 + 低 16 位
 *       每层：sum = (sum & Mk) + ((sum >> shift) & Mk)
 *
 * 验证：输入 15 → "4\n"; 输入 1024 → "1\n"
 */

#include <stdio.h>

#define M1	0x55555555
#define M2	0x33333333
#define M3	0x0F0F0F0F
#define M4	0x00FF00FF
#define M5	0x0000FFFF

int count_bits(int num)
{
	int sum = num;

#error TODO: Implement 5-layer divide-and-conquer popcount with masks M1-M5. Run "clings hint" for help.

	return sum;
}

int main(void)
{
	int num;

	scanf("%d", &num);
	printf("%d\n", count_bits(num));

	return 0;
}
