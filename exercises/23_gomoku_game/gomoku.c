/*
 * Lesson 23c: 完整游戏循环
 *
 * 知识点：while(1) 游戏循环，turn % 2 判断选手，break
 * 参考原课 gomoku.c 的 main 函数
 *
 * 任务：所有函数已实现，学生写 main 游戏循环:
 *       1. while (1) 循环
 *       2. 用 turn % 2 决定当前玩家 (X 或 O)
 *       3. scanf 读入 r c，验证合法性
 *       4. 落子 board[r][c] = cur
 *       5. 调用 check_win(cur)，赢了打印 "Player X wins!\n" 并 return
 *       6. 调用 board_full()，满了打印 "Draw!\n" 并 return
 *       7. turn++
 *
 * 验证：输入 5 步连续 X 落子 → "X wins!\n"
 */

#include <stdio.h>
#include <string.h>

#define SIZE 15

static char board[SIZE][SIZE];

void init_board(void)  { memset(board, '.', sizeof(board)); }

int onboard(int r, int c) { return r >= 0 && r < SIZE && c >= 0 && c < SIZE; }
int empty(int r, int c)   { return board[r][c] == '.'; }

int check_dir(int r, int c, int dr, int dc, char player)
{
	int count = 0;
	for (int k = 0; k < 5; k++) {
		int nr = r + k * dr, nc = c + k * dc;
		if (nr < 0 || nr >= SIZE || nc < 0 || nc >= SIZE) return 0;
		if (board[nr][nc] == player) count++; else break;
	}
	return count >= 5;
}

int check_win(char player)
{
	int dirs[][2] = {{0,1},{1,0},{1,1},{1,-1}};
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++) {
			if (board[i][j] != player) continue;
			for (int d = 0; d < 4; d++)
				if (check_dir(i, j, dirs[d][0], dirs[d][1], player)) return 1;
		}
	return 0;
}

int board_full(void)
{
	for (int i = 0; i < SIZE; i++)
		for (int j = 0; j < SIZE; j++)
			if (board[i][j] == '.') return 0;
	return 1;
}

int main(void)
{
	init_board();

#error TODO: Implement game loop: alternate players, place, check win/draw. Run "clings hint" for help.

	return 0;
}
