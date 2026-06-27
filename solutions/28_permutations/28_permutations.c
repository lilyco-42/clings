/* 28_permutations.c — 全排列问题（参考解答） */
#include <stdio.h>
#include <string.h>

void swap(char *a, char *b) {
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

void permute(char *str, int l, int r) {
    if (l == r) {
        printf("%s\n", str);
        return;
    }
    for (int i = l; i <= r; i++) {
        swap(&str[l], &str[i]);
        permute(str, l + 1, r);
        swap(&str[l], &str[i]); /* 回溯 */
    }
}

int main(void) {
    char str[256];
    fgets(str, sizeof(str), stdin);
    int len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
    permute(str, 0, strlen(str) - 1);
    return 0;
}
