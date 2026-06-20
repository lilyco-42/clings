/*
 * Lesson 29a: 文件行读取与动态内存 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 65536

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
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

    printf("lines: %d\n", lines);
    for (int i = 0; i < lines; i++)
        printf("%s", pv[i]);

    for (int i = 0; i < lines; i++)
        free(pv[i]);

    return 0;
}
