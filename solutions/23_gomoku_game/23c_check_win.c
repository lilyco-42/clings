#include <stdio.h>
#include <string.h>

#define SIZE 15

char board[SIZE][SIZE + 1];

int check_dir(int r, int c, int dr, int dc, char player)
{
	int count = 0;
	for (int k = 0; k < 5; k++)
	{
		int nr = r + k * dr, nc = c + k * dc;
		if (nr < 0 || nr >= SIZE || nc < 0 || nc >= SIZE) return 0;
		if (board[nr][nc] == player) count++;
		else break;
	}
	return count >= 5;
}

int check_win(char player)
{
	int dirs[][2] = {{0,1}, {1,0}, {1,1}, {1,-1}};
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
		{
			if (board[i][j] != player) continue;
			for (int d = 0; d < 4; d++)
				if (check_dir(i, j, dirs[d][0], dirs[d][1], player))
					return 1;
		}
	return 0;
}

int main(void)
{
	for (int i = 0; i < SIZE; i++)
		scanf("%s", board[i]);

	if (check_win('B'))
		printf("black\n");
	else if (check_win('W'))
		printf("white\n");
	else
		printf("none\n");

	return 0;
}
