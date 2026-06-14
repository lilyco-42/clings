#include <stdio.h>
#include <string.h>

#define SIZE 15

static char board[SIZE][SIZE];

void init_board(void)
{
	memset(board, '.', sizeof(board));
}

void print_board(void)
{
	printf("   ");
	for (int j = 0; j < SIZE; j++)
		printf("%2d", j);
	printf("\n");
	for (int i = 0; i < SIZE; i++)
	{
		printf("%2d ", i);
		for (int j = 0; j < SIZE; j++)
			printf(" %c", board[i][j]);
		printf("\n");
	}
}

int main(void)
{
	init_board();
	print_board();

	return 0;
}
