/*
 * Lesson 44a: 朴素字符串匹配 — 参考答案
 */
#include <stdio.h>
#include <string.h>

void naive_search(const char *text, const char *pattern)
{
    int n = (int)strlen(text);
    int m = (int)strlen(pattern);
    int found = 0;

    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++) {
            if (text[i + j] != pattern[j]) break;
        }
        if (j == m) {
            if (found > 0) printf(" ");
            printf("%d", i);
            found++;
        }
    }
    if (found == 0) printf("-1");
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 3) return 1;
    naive_search(argv[1], argv[2]);
    return 0;
}
