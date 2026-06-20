// 31b_file_time.c — 格式化文件修改时间
//
// 任务: 用 stat 获取文件的最后修改时间，用 strftime 格式化输出。
//
// 知识点: localtime()、strftime()、struct tm
//
// 用法: ./31b_file_time <filename>
// 输出: "mtime: YYYY-MM-DD HH:MM:SS\n"

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // TODO: stat(argv[1], &buf)，失败时 perror + return 1
    // TODO: 用 localtime(&buf.st_mtime) 转换为本地时间
    // TODO: 用 strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", loctime) 格式化
    // TODO: printf("mtime: %s\n", buffer)

    return 0;
}
