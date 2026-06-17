/*
 * Lesson 14b: 方向检查 — struct direction + check()
 *
 * 知识点：struct, typedef, 函数参数传结构体，方向数组
 * 参考原课 solution.c 的 struct direction 和 check() 函数
 *
 * 任务：实现 check(row, col, dir) 函数:
 *       1. 计算 nr = row + dir.dr, nc = col + dir.dc
 *       2. 用 is_valid 检查 (nr, nc) 是否在棋盘内
 *       3. 检查 chessboard[nr][nc] == 0 (可通行)
 *       4. 两个条件都满足返回 1，否则返回 0
 *
 * 验证：位置 (1,1) → "direction up is ok!\n..."
 */

#include <stdio.h>

#define ROW	5
#define COL	5

int chessboard[ROW][COL];

/* 平台无关的伪随机数生成器 (Linear Congruential Generator) */
static unsigned int _seed = 42;
int my_rand(void) { _seed = _seed * 1103515245 + 12345; return (_seed >> 16) & 0x7fff; }

void init_chessboard(void)
{
	int i, j;

	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			chessboard[i][j] = my_rand() % 2;

	return;
}

int is_valid(int row, int col)
{
	if (row < 0 || row >= ROW)
		return 0;

	if (col < 0 || col >= COL)
		return 0;

	return 1;
}

struct direction
{
	int dr;
	int dc;
	char name[16];
};

typedef struct direction dir_t;

dir_t dirs[4] =
{
	{-1, 0, "up"},
	{1, 0, "down"},
	{0, -1, "left"},
	{0, 1, "right"},
};

int check(int row, int col, dir_t dir)
{
#error TODO: Compute new pos, check is_valid && chessboard==0, return 1 if ok. Run "clings hint" for help.
}

int main(void)
{
	int row, col;
	int ways = 0;
	int i;

	init_chessboard();

	scanf("%d %d", &row, &col);

	for (i = 0; i < 4; i++)
	{
		int ret;

		ret = check(row, col, dirs[i]);
		if (ret > 0)
		{
			printf("direction %s is ok!\n", dirs[i].name);
			ways++;
		}
	}

	printf("value = %d\n", chessboard[row][col]);
	printf("ways = %d\n", ways);

	return 0;
}
