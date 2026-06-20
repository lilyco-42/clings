// 31a_file_size.c — 使用 stat 获取文件大小
//
// 任务: 用 stat() 系统调用获取文件大小（字节数），打印输出。
//
// 知识点: stat() 函数、struct stat、st_size 字段
//
// 用法: ./31a_file_size <filename>
// 输出: "size: N\n" 其中 N 是文件字节数

#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // TODO: 声明 struct stat buf
    // TODO: 调用 stat(argv[1], &buf)，失败时 perror + return 1
    // TODO: 打印 "size: %lld\n"，注意用 (long long)buf.st_size 强转

    return 0;
}
