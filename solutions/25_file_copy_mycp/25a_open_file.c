/*
 * Lesson 25a: 文件打开与错误处理 — 参考答案
 */
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror(argv[1]);
        return 1;
    }

    printf("open OK\n");
    fclose(fp);
    return 0;
}
