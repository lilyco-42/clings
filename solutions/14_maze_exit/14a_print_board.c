#include <stdio.h>

#define ROW 5
#define COL 5

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
}

void print_chessboard(void)
{
	int i, j;

	for (i = 0; i < ROW; i++)
	{
		for (j = 0; j < COL; j++)
		{
			if (j > 0) printf(" ");
			printf("%d", chessboard[i][j]);
		}
		printf("\n");
	}
}

int main(void)
{
	init_chessboard();
	print_chessboard();

	return 0;
}
