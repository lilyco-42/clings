/*
 * Lesson 08: 1 到 100 有多少个 9
 *
 * 知识点：函数 Function, 形参和实参，函数返回值，逻辑分解，注释的写法
 *
 * 任务：参考原课 sum9.c 的注释风格，实现 find 函数:
 *       1. find(num, digit) 用 do-while 循环拆位
 *       2. 每次取 num % 10 与 digit 比较，相等则 counter++
 *       3. 然后 num /= 10，直到 num == 0
 *       4. main 中调用 find 已写好，只需实现函数体
 *
 * 课堂讨论：find 中的 do-while 改成 while 可以吗？
 *           为什么不写一个函数直接计算 1-100 中 9 的个数？
 *
 * 验证：stdout 精确匹配 "sum = 20\n"
 */

#include <stdio.h>

/*
 * find - calculate how many digit in num
 * @num:	the number we want to find
 * @digit:	the digit we search in num
 *
 * Return value:  how many digit in this num
 */
int find(int num, int digit)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

int main(void)
{
	int begin = 1;
	int end = 100;
	int i = 0;
	int sum = 0;

	for (i = begin; i <= end; i++)
	{
		sum += find(i, 9);
	}

	printf("sum = %d\n", sum);

	return 0;
}
