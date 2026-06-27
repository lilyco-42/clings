/* 25_my_strstr.c — 实现 my_strstr：暴力字符串匹配（参考解答） */
#include <stdio.h>
#include <string.h>

char *my_strstr(const char *haystack, const char *needle) {
    /* 空串直接返回 haystack（与标准 strstr 行为一致） */
    if (*needle == '\0') return (char *)haystack;
    int n = strlen(haystack), m = strlen(needle);
    for (int i = 0; i <= n - m; i++) {
        int j;
        for (j = 0; j < m; j++)
            if (haystack[i + j] != needle[j]) break;
        if (j == m) return (char *)&haystack[i];
    }
    return NULL;
}

int main(void) {
    char haystack[256], needle[256];
    fgets(haystack, sizeof(haystack), stdin);
    fgets(needle, sizeof(needle), stdin);
    for (int i = 0; haystack[i]; i++)
        if (haystack[i] == '\n') haystack[i] = '\0';
    for (int i = 0; needle[i]; i++)
        if (needle[i] == '\n') needle[i] = '\0';

    char *result = my_strstr(haystack, needle);
    if (result)
        printf("found: %s\n", result);
    else
        printf("not found\n");
    return 0;
}
