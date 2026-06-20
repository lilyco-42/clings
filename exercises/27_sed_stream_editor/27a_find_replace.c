/*
 * Lesson 27a: 字符串查找与替换（第一次出现）
 *
 * 知识点：strstr 查找子串, 指针算术, putchar 逐字符输出
 *
 * 任务：
 *   从 stdin 读取一行文本，将第一次出现的 argv[1] 替换为 argv[2]，
 *   将结果输出到 stdout。
 *
 *   示例：
 *     输入: "I love unix. unix is great.\n"
 *     argv: ["unix", "linux"]
 *     输出: "I love linux. unix is great.\n"
 *
 * 要点：
 *   - strstr(haystack, needle) 返回第一次匹配的指针，或 NULL
 *   - 替换逻辑：打印匹配前的部分 → 打印替换串 → 打印匹配后的部分
 */

#include <stdio.h>
#include <string.h>

#define SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s pattern replacement\n", argv[0]);
        return 1;
    }

    char buf[SIZE];
    if (fgets(buf, SIZE, stdin) == NULL)
        return 0;

    char *pattern = argv[1];
    char *replace = argv[2];
    size_t patlen = strlen(pattern);

#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 1. where = strstr(buf, pattern)
     * 2. 如果找到: 打印 buf 到 where 之间的字符, 打印 replace, 打印 where+patlen 之后的内容
     * 3. 如果没找到: 直接打印 buf */

    return 0;
}
