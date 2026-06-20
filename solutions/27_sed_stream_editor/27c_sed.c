/*
 * Lesson 27c: 完整 sed 流处理器 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s 's/pattern/replace/[flags]'\n", argv[0]);
        return 1;
    }

    char *cmd, *regexp, *replace, *flags = NULL;
    char flag;

    cmd = strtok(argv[1], "/");
    regexp = strtok(NULL, "/");
    replace = strtok(NULL, "/");
    flags = strtok(NULL, "/");

    if (cmd == NULL || regexp == NULL || replace == NULL) {
        fprintf(stderr, "invalid pattern\n");
        return 1;
    }

    flag = (flags == NULL) ? '1' : flags[0];

    int anchor_start = 0;
    char *pat = regexp;
    if (regexp[0] == '^') {
        anchor_start = 1;
        pat = regexp + 1;
    }
    size_t patlen = strlen(pat);

    if (patlen == 0) {
        char buf[SIZE];
        while (fgets(buf, SIZE, stdin) != NULL)
            fputs(buf, stdout);
        return 0;
    }

    char buf[SIZE];
    while (fgets(buf, SIZE, stdin) != NULL) {
        char *cursor = buf;
        char *where;
        int counter = 0;

        if (anchor_start) {
            if (strncmp(buf, pat, patlen) == 0) {
                fputs(replace, stdout);
                cursor = buf + patlen;
            }
            fputs(cursor, stdout);
            continue;
        }

        while ((where = strstr(cursor, pat)) != NULL) {
            counter++;
            fwrite(cursor, 1, where - cursor, stdout);
            if (flag == 'g' || flag - '0' == counter)
                fputs(replace, stdout);
            else
                fputs(pat, stdout);
            cursor = where + patlen;
        }
        fputs(cursor, stdout);
    }

    return 0;
}
