/*
 * Lesson 22b: 比较猜测与答案 — ?A?B
 *
 * 知识点：双重循环，指针参数 *a *b, 数组比较
 * 参考原课 guess.c 中的 check() 函数
 *
 * 任务：实现 check(secret, guess, &a, &b):
 *       1. 外层循环 i=0..3
 *       2. 如果 guess[i] == secret[i] → (*a)++ (位置和数字都对)
 *       3. 否则内层循环 j=0..3 找相同数字 → (*b)++ (数字对位置不对)
 *
 * 验证：secret=1234, guess=1325 → "1A2B\n"
 */

#include <stdio.h>

void check(const int secret[4], const int guess[4], int *a, int *b)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

int main(void)
{
	int secret[4] = {1, 2, 3, 4};
	int guess[4];
	int a, b;
	int i;
	char input[16];

	fgets(input, sizeof(input), stdin);
	for (i = 0; i < 4; i++)
		guess[i] = input[i] - '0';

	check(secret, guess, &a, &b);
	printf("%dA%dB\n", a, b);

	return 0;
}
