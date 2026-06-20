/*
 * Lesson 25a: 文件打开与错误处理
 *
 * 知识点：fopen, fclose, FILE*, NULL 检查, perror
 *
 * 任务：
 *   1. 从 argv[1] 获取文件名（若 argc < 2 则打印 Usage 并返回 1）
 *   2. 以 "rb" 模式打开文件
 *   3. 如果打开成功，打印 "open OK\n" 并关闭文件
 *   4. 如果打开失败，用 perror 打印错误信息并返回 1
 *
 * 验证：
 *   ./25a_open_file mycp.c       → "open OK\n"
 *   ./25a_open_file no_such_file → perror 输出（exit code 1）
 */

#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 用 fopen 以 "rb" 模式打开 argv[1]，
     * 成功则打印 "open OK\n" 并 fclose，
     * 失败则 perror(argv[1]) 并返回 1 */

    return 0;
}
