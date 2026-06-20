/*
 * Lesson 10a: 约瑟夫环 — 初始化与打印
 *
 * 知识点：全局数组初始化，for 循环，取模 %, 打印数组
 *
 * 任务：参考原课 josephus.c，实现 init_ring() + print_ring():
 *       1. init_ring(): next[i] = (i + 1) % n，建立循环链表
 *       2. print_ring(): 打印 next[0] 到 next[n-1]，每个数后跟一个空格，末尾换行
 *       3. main 中读入 n，初始化后打印
 *
 * 验证：输入 5 → "1 2 3 4 0 \n"; 输入 3 → "1 2 0 \n"
 *       (注意: 最后一个数后面也有空格)
 */

#include <stdio.h>

#define MAX 1000

int next[MAX];
int n;

void init_ring(void)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

void print_ring(void)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
}

int main(void)
{
	scanf("%d", &n);

	init_ring();
	print_ring();

	return 0;
}
