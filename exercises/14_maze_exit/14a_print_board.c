/*
 * Lesson 14a: 棋盘初始化与打印
 *
 * 知识点：二维数组，函数参数传数组，随机初始化
 *
 * 任务：实现 init_chessboard() + print_chessboard():
 *       1. init_chessboard(): 用 my_rand() % 2 填充 chessboard[ROW][COL]
 *       2. print_chessboard(): 二重 for 循环打印，格式 "%d" + 元素间空格 + 每行换行
 *
 * 注意：使用 my_rand() 代替 stdlib rand()——rand() 在不同平台 (Windows/Linux)
 *       产生不同序列，自定义 LCG 保证跨平台输出一致。
 *       LCG 公式：seed = seed * 1103515245 + 12345 (glibc 经典参数)
 *
 * 验证：确定性输出 (见测试用例)
 */

#include <stdio.h>

#define ROW 5
#define COL 5

int chessboard[ROW][COL];

/* 平台无关的伪随机数生成器 (Linear Congruential Generator) */
static unsigned int _seed = 42;
int my_rand(void) { _seed = _seed * 1103515245 + 12345; return (_seed >> 16) & 0x7fff; }

void init_chessboard(void)
{
#error TODO: Fill chessboard[i][j] = my_rand() % 2 with nested loops. Run "clings hint" for help.
}

void print_chessboard(void)
{
#error TODO: Print board with nested loops, spaces between elements. Run "clings hint" for help.
}

int main(void)
{
	init_chessboard();
	print_chessboard();

	return 0;
}
