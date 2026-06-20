/*
 * Lesson 29b: 分页显示算法 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 65536

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <page_size>\n", argv[0]);
        return 1;
    }

    int page_size = atoi(argv[2]);
    if (page_size <= 0) {
        fprintf(stderr, "page_size must be positive\n");
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

    int dropdown = (lines < page_size) ? lines : page_size;
    for (int i = 0; i < dropdown; i++)
        printf("%s", pv[i]);

    for (int i = 0; i < lines; i++)
        free(pv[i]);

    return 0;
}
