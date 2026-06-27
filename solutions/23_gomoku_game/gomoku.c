#include <stdio.h>
#include <string.h>

#define SIZE 15

static char board[SIZE][SIZE];

void init_board(void) { memset(board, '.', sizeof(board)); }

int onboard(int r, int c) { return r >= 0 && r < SIZE && c >= 0 && c < SIZE; }
int empty(int r, int c) { return board[r][c] == '.'; }

int check_dir(int r, int c, int dr, int dc, char player) {
    int count = 0;
    for (int k = 0; k < 5; k++) {
        int nr = r + k * dr, nc = c + k * dc;
        if (nr < 0 || nr >= SIZE || nc < 0 || nc >= SIZE) return 0;
        if (board[nr][nc] == player)
            count++;
        else
            break;
    }
    return count >= 5;
}

int check_win(char player) {
    int dirs[][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != player) continue;
            for (int d = 0; d < 4; d++)
                if (check_dir(i, j, dirs[d][0], dirs[d][1], player)) return 1;
        }
    return 0;
}

int board_full(void) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == '.') return 0;
    return 1;
}

int main(void) {
    init_board();
    char players[] = {'X', 'O'};
    int turn = 0;

    while (1) {
        char cur = players[turn % 2];
        int r, c;

        if (scanf("%d %d", &r, &c) != 2) break;

        if (r < 0 || r >= SIZE || c < 0 || c >= SIZE) continue;
        if (board[r][c] != '.') continue;

        board[r][c] = cur;

        if (check_win(cur)) {
            printf("\nPlayer %c wins!\n", cur);
            return 0;
        }
        if (board_full()) {
            printf("\nDraw!\n");
            return 0;
        }
        turn++;
    }

    return 0;
}
