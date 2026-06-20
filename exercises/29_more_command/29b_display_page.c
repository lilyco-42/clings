// 29b_display_page.c — 分页显示文件内容
//
// 任务: 读取文件所有行到内存，按指定页大小（第二个参数）分页显示。
//       仅显示第一页内容，然后退出。
//
// 知识点: 分页算法（top + page_size 控制显示区域）
//
// 用法: ./29b_display_page <filename> <page_size>
// 输出: 打印第一页（最多 page_size 行），每行原样输出

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 65536

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <page_size>\n", argv[0]);
        return 1;
    }

    int page_size = atoi(argv[2]);
    if (page_size <= 0) {
        fprintf(stderr, "page_size must be positive\n");
        return 1;
    }

    // TODO: 打开文件，失败时 perror + return 1
    // TODO: 用 fgets + malloc 读取所有行到 pv[] 数组，统计 lines
    // TODO: 关闭文件
    // TODO: 计算第一页显示行数: dropdown = (lines < page_size) ? lines : page_size
    // TODO: 循环打印 pv[0] 到 pv[dropdown-1]
    // TODO: 释放所有 malloc 的内存

    return 0;
}
