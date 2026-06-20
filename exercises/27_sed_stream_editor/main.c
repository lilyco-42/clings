/*
 * main.c - sed 命令入口（供 Makefile 独立编译使用）
 *
 * 学生练习请使用 27a/27b/27c 文件。
 */
#include <stdio.h>

int sed_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    return sed_main(argc, argv);
}
