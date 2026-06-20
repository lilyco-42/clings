/*
 * Lesson 44c: KMP 字符串匹配 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define MAX_LEN 1024

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

int kmp_search(const char *text, const char *pattern)
{
    int n = (int)strlen(text);
    int m = (int)strlen(pattern);
    build_next(pattern, m);

    int i = 0, j = 0;
    while (i < n) {
        if (text[i] == pattern[j]) {
            i++;
            j++;
        }
        if (j == m) {
            return i - j;
        }
        if (i < n && text[i] != pattern[j]) {
            if (j > 0)
                j = next_table[j - 1];
            else
                i++;
        }
    }
    return -1;
}

int main(int argc, char *argv[])
{
    if (argc < 3) return 1;
    printf("%d\n", kmp_search(argv[1], argv[2]));
    return 0;
}
