// 41d_bsearch_generic.c — 泛型二分查找 (void* + 比较函数)
//
// 任务: 实现通用二分查找，支持任意类型数组
//       在字符串数组 ["apple","banana","cherry","date","fig"] 中查找目标
//
// 知识点: void* 泛型编程、函数指针、comparator 模式
//
// 用法: ./41d_bsearch_generic <word>
// 示例: ./41d_bsearch_generic cherry → 输出 "found at 2"
//       ./41d_bsearch_generic grape  → 输出 "not found"

#include <stdio.h>
#include <string.h>

// TODO: 实现泛型二分查找函数
// 功能: 在 base 指向的有序数组中查找 key
// 参数:
//   key   — 要查找的元素指针
//   base  — 数组首地址
//   n     — 元素个数
//   size  — 每个元素的字节大小
//   cmp   — 比较函数: cmp(a, b) < 0 表示 a < b
// 返回: 找到返回元素指针，未找到返回 NULL
//
// 提示: 第 i 个元素地址 = (char*)base + i * size
//
// void *generic_bsearch(const void *key, const void *base,
//                       int n, int size,
//                       int (*cmp)(const void *, const void *)) { ... }

// TODO: 实现字符串比较函数 (用于 comparator)
// 注意: 参数是 const char** 类型（指向字符串指针的指针）
// int cmp_str(const void *a, const void *b) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    const char *words[] = {"apple", "banana", "cherry", "date", "fig"};
    int n = 5;
    const char *target = argv[1];

    // TODO: 调用 generic_bsearch 查找 target
    // void *result = generic_bsearch(&target, words, n, sizeof(const char *), cmp_str);
    // if (result) {
    //     int index = (int)((const char **)result - words);
    //     printf("found at %d\n", index);
    // } else {
    //     printf("not found\n");
    // }
    (void)words; (void)n; (void)target;

    return 0;
}
