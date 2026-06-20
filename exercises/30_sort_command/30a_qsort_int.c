// 30a_qsort_int.c — 用 qsort 对整数数组排序
//
// 任务: 从命令行参数读取整数，用 qsort 排序后输出。
//
// 知识点: qsort 函数签名、比较函数的编写（const void* 转换）
//
// 用法: ./30a_qsort_int 5 3 8 1 4
// 输出: 每行一个数字，升序排列

#include <stdio.h>
#include <stdlib.h>

// TODO: 实现比较函数 cmp_int(const void *a, const void *b)
//       1. 将 const void* 转为 const int*
//       2. 返回 (*pa > *pb) - (*pa < *pb) 避免减法溢出
//       提示: 不要用 *pa - *pb，大数相减会溢出!

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num1> <num2> ...\n", argv[0]);
        return 1;
    }

    int n = argc - 1;

    // TODO: 声明 int arr[n] 或 malloc 分配
    // TODO: 用 atoi(argv[i+1]) 填充数组
    // TODO: 调用 qsort(arr, n, sizeof(int), cmp_int)
    // TODO: 循环打印每个元素，格式 "%d\n"

    return 0;
}
