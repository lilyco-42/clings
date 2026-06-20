/*
 * Lesson 44b: 构建 KMP next 表 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define MAX_LEN 256

int next_table[MAX_LEN];

void build_next(const char *pattern, int m)
{
    next_table[0] = 0;
    int len = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[len]) {
            len++;
            next_table[i] = len;
            i++;
        } else {
            if (len > 0) {
                len = next_table[len - 1];
            } else {
                next_table[i] = 0;
                i++;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    const char *pattern = argv[1];
    int m = (int)strlen(pattern);

    build_next(pattern, m);

    for (int i = 0; i < m; i++)
        printf("%d%c", next_table[i], i < m - 1 ? ' ' : '\n');

    return 0;
}
