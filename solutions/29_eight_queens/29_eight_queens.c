/* 29_eight_queens.c — 八皇后问题（参考解答） */
#include <stdio.h>
#include <stdlib.h>

int count = 0;
int col[8];

int is_safe(int row, int c) {
    for (int i = 0; i < row; i++)
        if (col[i] == c || abs(row - i) == abs(c - col[i])) return 0;
    return 1;
}

void solve(int row) {
    if (row == 8) {
        count++;
        return;
    }
    for (int c = 0; c < 8; c++) {
        if (is_safe(row, c)) {
            col[row] = c;
            solve(row + 1);
        }
    }
}

int main(void) {
    count = 0;
    solve(0);
    printf("%d\n", count);
    return 0;
}
