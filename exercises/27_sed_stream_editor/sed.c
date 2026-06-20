/*
 * sed.c - 参考实现：简化版 sed 流处理器
 *
 * 支持: s/regexp/replacement/flags 格式
 * flags: 无(第1个), g(全部), N(第N个)
 * 支持: ^ 行首锚定
 *
 * 本文件为 Lesson 27 的参考代码，供学生阅读学习。
 * 学生练习文件为 27a/27b/27c。
 */
#include <stdio.h>
#include <string.h>

#define SIZE 512

int sed_main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s 's/pattern/replace/[flags]'\n", argv[0]);
        return 1;
    }

    char *cmd, *regexp, *replace, *flags = NULL;
    char flag;

    /* 解析 s/regexp/replace/flags */
    cmd = strtok(argv[1], "/");
    regexp = strtok(NULL, "/");
    replace = strtok(NULL, "/");
    flags = strtok(NULL, "/");

    if (cmd == NULL || regexp == NULL || replace == NULL) {
        fprintf(stderr, "invalid pattern\n");
        return 1;
    }

    flag = (flags == NULL) ? '1' : flags[0];

    /* 检查 ^ 行首锚定 */
    int anchor_start = 0;
    char *pat = regexp;
    if (regexp[0] == '^') {
        anchor_start = 1;
        pat = regexp + 1;
    }
    size_t patlen = strlen(pat);

    if (patlen == 0) {
        /* 空正则: 直接透传所有输入 */
        char buf[SIZE];
        while (fgets(buf, SIZE, stdin) != NULL)
            fputs(buf, stdout);
        return 0;
    }

    /* 逐行处理 */
    char buf[SIZE];
    while (fgets(buf, SIZE, stdin) != NULL) {
        char *cursor = buf;
        char *where;
        int counter = 0;

        if (anchor_start) {
            /* ^ 锚定: 只检查行首是否匹配 */
            if (strncmp(buf, pat, patlen) == 0) {
                fputs(replace, stdout);
                cursor = buf + patlen;
            }
            fputs(cursor, stdout);
            continue;
        }

        /* 普通替换 */
        while ((where = strstr(cursor, pat)) != NULL) {
            counter++;

            /* 打印匹配前的部分 */
            for (char *p = cursor; p < where; p++)
                putchar(*p);

            /* 根据 flag 决定是否替换 */
            if (flag == 'g' || flag - '0' == counter)
                fputs(replace, stdout);
            else
                fputs(pat, stdout);

            cursor = where + patlen;
        }

        /* 打印剩余部分 */
        fputs(cursor, stdout);
    }

    return 0;
}
