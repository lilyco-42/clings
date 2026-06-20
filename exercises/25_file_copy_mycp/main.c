/*
 * main.c - mycp 命令入口（供 Makefile 独立编译使用）
 *
 * 学生练习请使用 25a/25b/25c 文件。
 */
#include <stdio.h>

int mycp_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    return mycp_main(argc, argv);
}
