// 41a_bsearch_iter.c — 迭代二分查找
//
// 任务: 在有序数组 [1,3,5,7,9,11] 中查找命令行指定的目标值
//       找到返回下标，未找到返回 -1
//
// 知识点: 二分查找的迭代实现、lo/hi/mid 指针
//
// 用法: ./41a_bsearch_iter <target>
// 示例: ./41a_bsearch_iter 5  → 输出 "2"
//       ./41a_bsearch_iter 6  → 输出 "-1"

#include <stdio.h>
#include <stdlib.h>

// TODO: 实现迭代二分查找函数
// 功能: 在有序数组 arr[0..n-1] 中查找 target
//   1. 初始化 lo=0, hi=n-1
//   2. 当 lo <= hi:
//      a. mid = lo + (hi - lo) / 2
//      b. 如果 arr[mid] == target，返回 mid
//      c. 如果 arr[mid] < target，lo = mid + 1
//      d. 否则 hi = mid - 1
//   3. 未找到返回 -1
// int binary_search(int arr[], int n, int target) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int arr[] = {1, 3, 5, 7, 9, 11};
    int n = 6;
    int target = atoi(argv[1]);

    // TODO: 调用 binary_search 并打印结果
    // printf("%d\n", binary_search(arr, n, target));
    (void)arr; (void)n; (void)target;

    return 0;
}
