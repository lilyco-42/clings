/*
 * Lesson 27a: 字符串查找与替换 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s pattern replacement\n", argv[0]);
        return 1;
    }

    char buf[SIZE];
    if (fgets(buf, SIZE, stdin) == NULL)
        return 0;

    char *pattern = argv[1];
    char *replace = argv[2];
    size_t patlen = strlen(pattern);

    char *where = strstr(buf, pattern);
    if (where != NULL) {
        fwrite(buf, 1, where - buf, stdout);
        fputs(replace, stdout);
        fputs(where + patlen, stdout);
    } else {
        fputs(buf, stdout);
    }

    return 0;
}
