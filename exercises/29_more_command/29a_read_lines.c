// 29a_read_lines.c — 从文件读取所有行到内存
//
// 任务: 读取文件的每一行，用 malloc 分配内存保存，
//       最后打印总行数和所有行的内容。
//
// 知识点: fgets + malloc + strcpy，指针数组管理
//
// 用法: ./29a_read_lines <filename>
// 输出: 先打印 "lines: N"，再打印所有行内容

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 65536

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    // TODO: 打开文件（只读文本模式），失败时用 perror 报错并返回 1
    // TODO: 声明行指针数组 char *pv[MAX_LINES] 和行计数器 int lines = 0
    // TODO: 用 fgets 循环读取每一行到临时缓冲区
    //       对每一行: malloc(strlen(buf) + 1)，strcpy 保存，lines++
    // TODO: 关闭文件
    // TODO: 打印 "lines: %d\n" 格式的总行数
    // TODO: 循环打印每一行内容（直接 printf("%s", pv[i])）
    // TODO: 释放所有 malloc 的内存

    return 0;
}
