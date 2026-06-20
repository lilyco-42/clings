/*
 * Lesson 31c: ls -l 完整文件信息 — 参考答案 (Linux-only)
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [-i] [-s] [-T]\n", argv[0]);
        return 1;
    }

    struct stat buf;
    if (stat(argv[1], &buf) != 0) {
        perror(argv[1]);
        return 1;
    }

    int opt_i = 0, opt_s = 0, opt_T = 0;
    for (int i = 2; i < argc; i++) {
        if (argv[i][0] != '-') continue;
        for (const char *p = argv[i] + 1; *p; p++) {
            switch (*p) {
            case 'i': opt_i = 1; break;
            case 's': opt_s = 1; break;
            case 'T': opt_T = 1; break;
            }
        }
    }

    struct tm *loctime = localtime(&buf.st_mtime);

    if (opt_i) {
        printf("file st_ino = %llu\n", (unsigned long long)buf.st_ino);
        return 0;
    }
    if (opt_s) {
        printf("file st_blocks = %lld\n", (long long)buf.st_blocks);
        return 0;
    }
    if (opt_T) {
        char buffer[256];
        strftime(buffer, sizeof(buffer),
                 "The time is %m %d %H:%M:%S %Y\n", loctime);
        fputs(buffer, stdout);
        return 0;
    }

    printf("Access mode: 0%o\n", buf.st_mode);

    struct passwd *pw = getpwuid(buf.st_uid);
    printf("file uid = %s\n", pw ? pw->pw_name : "unknown");

    printf("file size = %lld\n", (long long)buf.st_size);
    printf("file last modify mtime = %s", ctime(&buf.st_mtime));

    char buffer[256];
    strftime(buffer, sizeof(buffer),
             "The time is %m %d %H:%M:%S %Y\n", loctime);
    fputs(buffer, stdout);

    return 0;
}
