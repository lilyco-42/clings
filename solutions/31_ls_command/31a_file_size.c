/*
 * Lesson 31a: stat 获取文件大小 — 参考答案
 */
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    struct stat buf;
    if (stat(argv[1], &buf) != 0) {
        perror(argv[1]);
        return 1;
    }

    printf("size: %lld\n", (long long)buf.st_size);
    return 0;
}
