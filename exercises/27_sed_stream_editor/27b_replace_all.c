/*
 * Lesson 27b: 全局替换（/g 标志）
 *
 * 知识点：循环 strstr, cursor 指针前进, 替换计数
 *
 * 任务：
 *   从 stdin 读取一行文本，根据 argv[3]（flags）决定替换行为：
 *   - 无 flag 或 "1": 只替换第一次出现
 *   - "g": 替换所有出现
 *   - "N"（数字）: 只替换第 N 次出现
 *
 *   用法: ./27b_replace_all pattern replacement [flags]
 *
 *   示例：
 *     echo "unix unix unix" | ./27b_replace_all unix linux g
 *     输出: "linux linux linux\n"
 *
 *     echo "unix unix unix" | ./27b_replace_all unix linux 2
 *     输出: "unix linux unix\n"
 */

#include <stdio.h>
#include <string.h>

#define SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s pattern replacement [g|N]\n", argv[0]);
        return 1;
    }

    char buf[SIZE];
    if (fgets(buf, SIZE, stdin) == NULL)
        return 0;

    char *pattern = argv[1];
    char *replace = argv[2];
    char flag = (argc > 3) ? argv[3][0] : '1';
    size_t patlen = strlen(pattern);

    if (patlen == 0) {
        fputs(buf, stdout);
        return 0;
    }

#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 核心算法：
     * char *cursor = buf;
     * int counter = 0;
     * while ((where = strstr(cursor, pattern)) != NULL) {
     *     counter++;
     *     // 打印 cursor 到 where 之间的字符
     *     // 如果 flag=='g' 或 flag-'0'==counter: 打印 replace
     *     // 否则: 打印 pattern 原文
     *     cursor = where + patlen;
     * }
     * // 打印 cursor 之后的剩余内容
     */

    return 0;
}
