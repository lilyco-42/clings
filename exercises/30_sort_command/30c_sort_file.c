// 30c_sort_file.c — 从文件读取行并排序输出（支持 -u -r -n 选项）
//
// 任务: 实现简化版 sort 命令。
//       从文件读取所有行，排序后输出。
//       支持: -u 去重、-r 逆序、-n 数值排序
//
// 知识点: 综合 qsort + 函数指针 + 文件读取 + 参数解析
//
// 用法: ./30c_sort_file <filename> [-u] [-r] [-n]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 65536

// TODO: 实现三个比较函数:
//   cmp_str   — strcmp 字典序
//   cmp_str_r — 逆序（返回 -strcmp 的结果）
//   cmp_str_n — 数值排序（atoi 后用 (a>b)-(a<b) 比较，避免溢出）

// TODO: 实现 display(char *pv[], int lines, int unique)
//   如果 unique 为 1，跳过与上一行相同的行

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename> [-u] [-r] [-n]\n", argv[0]);
        return 1;
    }

    // TODO: 打开文件，fgets + malloc 读取所有行
    // TODO: 关闭文件
    // TODO: 解析选项（遍历 argv[2..argc-1]，支持任意顺序）
    // TODO: 根据选项选择比较函数，调用 qsort
    // TODO: 调用 display 输出结果
    // TODO: 释放内存

    return 0;
}
