// 42c_qsort_generic.c — 泛型快速排序 (排序字符串数组)
//
// 任务: 实现支持任意类型的快速排序，对命令行参数（字符串）进行排序
//
// 知识点: void* 泛型、memcpy 交换、函数指针
//
// 用法: ./42c_qsort_generic banana apple cherry date
// 预期输出: apple banana cherry date

#include <stdio.h>
#include <string.h>

// TODO: 实现泛型 swap 函数
// 功能: 交换 a 和 b 指向的 size 字节数据
// void generic_swap(void *a, void *b, int size) {
//     char tmp[256];
//     memcpy(tmp, a, size);
//     memcpy(a, b, size);
//     memcpy(b, tmp, size);
// }

// TODO: 实现泛型 partition 函数
// 提示: 用 (char*)base + i * size 计算第 i 个元素地址
// int generic_partition(void *base, int lo, int hi, int size,
//                       int (*cmp)(const void *, const void *)) { ... }

// TODO: 实现泛型 quicksort 函数
// void generic_qsort(void *base, int lo, int hi, int size,
//                    int (*cmp)(const void *, const void *)) { ... }

// TODO: 实现字符串比较函数
// int cmp_str(const void *a, const void *b) {
//     return strcmp(*(const char **)a, *(const char **)b);
// }

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int n = argc - 1;
    const char *words[64];
    for (int i = 0; i < n; i++)
        words[i] = argv[i + 1];

    // TODO: 调用 generic_qsort(words, 0, n - 1, sizeof(const char *), cmp_str)
    // TODO: 打印排序后数组（空格分隔，末尾换行）
    // for (int i = 0; i < n; i++)
    //     printf("%s%c", words[i], i < n-1 ? ' ' : '\n');
    (void)words; (void)n;

    return 0;
}
