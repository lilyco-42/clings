/*
 * Lesson 10: 约瑟夫环 — 链表模拟
 *
 * 知识点：全局数组 next[] 模拟循环链表，while 循环，链表出队
 * 参考原课 josephus.c — 用 next[] 记录下一人的编号
 *
 * 任务：实现约瑟夫环的链表出队逻辑:
 *       1. while (left > 0) 循环
 *       2. counter++ 计数
 *       3. 当 counter == OUT 时:
 *          - printf("%d is out\n", i + 1) 打印出局者
 *          - next[prev] = next[i] 将 i 从链表中删除
 *          - left--, counter = 0
 *       4. prev = i; i = next[i]; 前进到下一人
 *
 * 验证：ALL=10, OUT=3 → "3 is out\n6 is out\n...\n4 is out\n"
 */

#include <stdio.h>

#define ALL	10
#define OUT	3

int next[ALL];

void init_ring(void)
{
	int i;
	for (i = 0; i < ALL; i++)
		next[i] = (i + 1) % ALL;
}

int main(void)
{
	int left;
	int counter;
	int i;
	int prev;

	init_ring();

#error TODO: Implement Josephus ring elimination loop. Run "clings hint" for help.
	/* 提示:
	 *   counter++;
	 *   if (counter == OUT) {
	 *       left--;
	 *       printf("%d is out\n", i + 1);
	 *       next[prev] = next[i];
	 *       counter = 0;
	 *   }
	 *   prev = i;
	 *   i = next[i];
	 */

	return 0;
}
