// 32b_dispatch.c — 函数指针分发表
//
// 任务: 实现命令分发机制。根据命令名从分发表中查找并调用对应函数。
//       内置 3 个命令: hello/add/echo
//
// 知识点: 结构体数组、函数指针、命令分发模式
//
// 用法: echo "add 3 5" | ./32b_dispatch
// 输出: 8
//
// 用法: echo "hello" | ./32b_dispatch
// 输出: Hello, NCCL!
//
// 用法: echo "echo foo bar" | ./32b_dispatch
// 输出: foo bar

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 32

// --- 内置命令实现 ---

static int cmd_hello(int argc, char *argv[])
{
    (void)argc; (void)argv;
    printf("Hello, NCCL!\n");
    return 0;
}

static int cmd_add(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: add <a> <b>\n");
        return 1;
    }
    printf("%d\n", atoi(argv[1]) + atoi(argv[2]));
    return 0;
}

static int cmd_echo(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if (i > 1) printf(" ");
        printf("%s", argv[i]);
    }
    printf("\n");
    return 0;
}

// --- 分发表 ---
// TODO: 定义 struct operation { char name[16]; int (*pf)(int, char **); };
// TODO: 定义 op[] 数组，包含 hello/add/echo 三条记录

// TODO: 实现 command_do(int argc, char *argv[])
//       遍历 op[] 数组，strcmp 匹配命令名，调用对应函数
//       找不到时打印 "command not found: %s\n"

// TODO: 实现 shell_parse(char *buf)
//       将 buf 解析为 argc/argv（同 32a），然后调用 command_do

int main(void)
{
    char buf[256];

    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;

    // TODO: 调用 shell_parse(buf)

    return 0;
}
