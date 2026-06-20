/*
 * Lesson 30b: qsort 字符串排序 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int cmp_str(const void *a, const void *b)
{
    const char *s1 = *(const char **)a;
    const char *s2 = *(const char **)b;
    return strcmp(s1, s2);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <str1> <str2> ...\n", argv[0]);
        return 1;
    }

    qsort(argv + 1, argc - 1, sizeof(char *), cmp_str);

    for (int i = 1; i < argc; i++)
        printf("%s\n", argv[i]);

    return 0;
}
