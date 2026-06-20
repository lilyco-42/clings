// 31c_ls_l.c — 完整 ls -l 命令实现（Linux-only）
//
// 任务: 实现 ls -l 的基本功能，支持 -i（inode）、-s（块数）、-T（完整时间）。
//
// 知识点: 综合 stat + localtime + strftime + getpwuid
//
// 编译: gcc -Wall -Wextra -std=c11 -g 31c_ls_l.c -o 31c_ls_l
// 用法: ./31c_ls_l <filename> [-i] [-s] [-T]

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

    // TODO: stat(argv[1], &buf)，失败时 perror + return 1

    // TODO: 解析选项 -i, -s, -T（遍历 argv[2..])

    // TODO: localtime(&buf.st_mtime) 得到本地时间

    // TODO: 如果 -i: printf("file st_ino = %llu\n", (unsigned long long)buf.st_ino)
    // TODO: 如果 -s: printf("file st_blocks = %lld\n", (long long)buf.st_blocks)
    // TODO: 如果 -T: strftime 格式化并输出 "The time is MM DD HH:MM:SS YYYY"

    // TODO: 默认输出:
    //   Access mode: 0%o
    //   file uid = <username>  （用 getpwuid，注意判空！）
    //   file size = <bytes>
    //   file last modify mtime = <ctime 输出>
    //   The time is MM DD HH:MM:SS YYYY

    return 0;
}
