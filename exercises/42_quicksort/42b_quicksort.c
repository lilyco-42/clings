// 42b_quicksort.c — 完整快速排序
//
// 任务: 从命令行读取整数，用快速排序排序后输出
//
// 知识点: 分治法、递归、partition 复用
//
// 用法: ./42b_quicksort 5 3 8 1 4 2
// 预期输出: 1 2 3 4 5 8

#include <stdio.h>
#include <stdlib.h>

// TODO: 实现 swap 函数
// void swap(int *a, int *b) { ... }

// TODO: 实现 partition 函数 (Lomuto 方案，同 42a)
// int partition(int arr[], int lo, int hi) { ... }

// TODO: 实现 quicksort 递归函数
// 功能:
//   1. 如果 lo >= hi，直接返回（递归终止）
//   2. pivot_idx = partition(arr, lo, hi)
//   3. 递归排序左半: quicksort(arr, lo, pivot_idx - 1)
//   4. 递归排序右半: quicksort(arr, pivot_idx + 1, hi)
// void quicksort(int arr[], int lo, int hi) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int n = argc - 1;
    int arr[64];
    for (int i = 0; i < n; i++)
        arr[i] = atoi(argv[i + 1]);

    // TODO: 调用 quicksort(arr, 0, n - 1)
    // TODO: 打印排序后数组（空格分隔，末尾换行）
    // for (int i = 0; i < n; i++)
    //     printf("%d%c", arr[i], i < n-1 ? ' ' : '\n');
    (void)arr; (void)n;

    return 0;
}
