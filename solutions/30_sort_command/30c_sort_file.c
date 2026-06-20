/*
 * Lesson 30c: sort 文件行排序命令 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 65536

static int cmp_str(const void *a, const void *b)
{
    const char *s1 = *(const char **)a;
    const char *s2 = *(const char **)b;
    return strcmp(s1, s2);
}

static int cmp_str_r(const void *a, const void *b)
{
    return -cmp_str(a, b);
}

static int cmp_str_n(const void *a, const void *b)
{
    int va = atoi(*(const char **)a);
    int vb = atoi(*(const char **)b);
    return (va > vb) - (va < vb);
}

static int cmp_str_nr(const void *a, const void *b)
{
    return -cmp_str_n(a, b);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [-u] [-r] [-n]\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror(argv[1]);
        return 1;
    }

    char *pv[MAX_LINES];
    int lines = 0;
    char buf[1024];

    while (fgets(buf, sizeof(buf), fp) && lines < MAX_LINES) {
        pv[lines] = malloc(strlen(buf) + 1);
        if (pv[lines] == NULL)
            break;
        strcpy(pv[lines], buf);
        lines++;
    }
    fclose(fp);

    int opt_u = 0, opt_r = 0, opt_n = 0;
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] != '-') continue;
        for (const char *p = argv[i] + 1; *p; p++) {
            switch (*p) {
            case 'u': opt_u = 1; break;
            case 'r': opt_r = 1; break;
            case 'n': opt_n = 1; break;
            }
        }
    }

    int (*cmp)(const void *, const void *) = cmp_str;
    if (opt_n && opt_r) cmp = cmp_str_nr;
    else if (opt_n)     cmp = cmp_str_n;
    else if (opt_r)     cmp = cmp_str_r;

    qsort(pv, lines, sizeof(char *), cmp);

    const char *prev = "";
    for (int i = 0; i < lines; i++) {
        if (opt_u && strcmp(pv[i], prev) == 0)
            continue;
        printf("%s", pv[i]);
        prev = pv[i];
    }

    for (int i = 0; i < lines; i++)
        free(pv[i]);

    return 0;
}
