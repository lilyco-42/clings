// 32a_shell_parse.c — 命令行字符串解析
//
// 任务: 将命令行字符串 "cmd arg1 arg2" 解析为 argc/argv。
//       从 stdin 读取一行，解析后打印 argc 和每个 argv。
//
// 知识点: 字符串原地分割（空格→'\0'）、指针数组
//
// 用法: echo "hello world foo" | ./32a_shell_parse
// 输出:
//   argc: 3
//   argv[0]: hello
//   argv[1]: world
//   argv[2]: foo

#include <stdio.h>
#include <string.h>

#define MAX_ARGS 32

int main(void)
{
    char buf[256];

    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;

    // TODO: 解析 buf 为 argc/argv
    //
    // 算法（状态机）:
    //   int in_word = 0;
    //   遍历 buf 的每个字符:
    //     - 如果是 '\n': 替换为 '\0' 并 break
    //     - 如果是空格/tab 且 in_word=1: 替换为 '\0'，in_word=0
    //     - 如果不是空格/tab 且 in_word=0: argv[argc++] = buf，in_word=1
    //   最后 argv[argc] = NULL
    //
    // TODO: 打印 "argc: %d\n"
    // TODO: 循环打印 "argv[%d]: %s\n"

    return 0;
}
