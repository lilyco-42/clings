/*
 * main.c - readelf 命令入口（供 Makefile 独立编译使用）
 *
 * 学生练习请使用 26a/26b/26c 文件。
 */
#include <stdio.h>

int readelf_main(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    return readelf_main(argc, argv);
}
