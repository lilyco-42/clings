/*
 * Lesson 27b: 全局替换 (/g 标志) — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s pattern replacement [g|N]\n", argv[0]);
        return 1;
    }

    char buf[SIZE];
    if (fgets(buf, SIZE, stdin) == NULL)
        return 0;

    char *pattern = argv[1];
    char *replace = argv[2];
    char flag = (argc > 3) ? argv[3][0] : '1';
    size_t patlen = strlen(pattern);

    if (patlen == 0) {
        fputs(buf, stdout);
        return 0;
    }

    char *cursor = buf;
    char *where;
    int counter = 0;

    while ((where = strstr(cursor, pattern)) != NULL) {
        counter++;
        fwrite(cursor, 1, where - cursor, stdout);
        if (flag == 'g' || flag - '0' == counter)
            fputs(replace, stdout);
        else
            fputs(pattern, stdout);
        cursor = where + patlen;
    }
    fputs(cursor, stdout);

    return 0;
}
