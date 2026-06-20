/*
 * Lesson 31b: 文件修改时间格式化 — 参考答案
 */
#include <stdio.h>
#include <time.h>
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

    struct tm *loctime = localtime(&buf.st_mtime);
    char buffer[256];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", loctime);
    printf("mtime: %s\n", buffer);

    return 0;
}
