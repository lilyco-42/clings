/* 43_strtok_r.c — strtok_r 线程安全字符串切分
 *
 * 任务: 使用 strtok_r 将命令行字符串按空格/tab 切分为独立参数
 *       存入 char *argv[] 数组（模拟 argc/argv），为 MyBash 热身
 *       正确处理连续空格（不产生空 token）和制表符
 *
 * 知识点: strtok_r API、分隔符字符串 " \t"、saveptr 上下文、
 *         strtok 线程不安全的根源（内部静态变量保存状态）
 *         strtok 通过写入 \0 修改原字符串——传入只读常量区会段错误
 *
 * 验证：
 *   stdin: "ls -l -a /tmp\n"     → argc: 4  argv: ls -l -a /tmp
 *   stdin: "echo   hello\n"      → argc: 2  （连续空格不产生空 token）
 *   stdin: "gcc\t-Wall\tmain.c\n" → argc: 3  （tab 分隔）
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

    char *argv[64], *saveptr;
    int argc = 0;

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 用 strtok_r 按 " \t" 切分 line：
     *   第一次: tok = strtok_r(line, " \t", &saveptr)
     *   之后: tok = strtok_r(NULL, " \t", &saveptr)
     *   每个非 NULL token 存入 argv[argc++]
     *
     * strtok_r 第三个参数 saveptr 保存内部状态 → 线程安全
     * （相比之下 strtok 用静态变量保存状态 → 多线程会互相干扰） */

    /* 打印 argc 和所有 argv */
    return 0;
}
