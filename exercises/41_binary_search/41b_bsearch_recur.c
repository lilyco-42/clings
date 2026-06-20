// 41b_bsearch_recur.c — 递归二分查找
//
// 任务: 用递归实现二分查找，在有序数组 [1,3,5,7,9,11] 中查找目标值
//
// 知识点: 递归分治思想、递归终止条件
//
// 用法: ./41b_bsearch_recur <target>
// 示例: ./41b_bsearch_recur 7  → 输出 "3"
//       ./41b_bsearch_recur 10 → 输出 "-1"

#include <stdio.h>
#include <stdlib.h>

// TODO: 实现递归二分查找函数
// 功能: 在 arr[lo..hi] 中递归查找 target
//   1. 终止条件: lo > hi 返回 -1
//   2. mid = lo + (hi - lo) / 2
//   3. 如果 arr[mid] == target，返回 mid
//   4. 如果 arr[mid] < target，递归查找 arr[mid+1..hi]
//   5. 否则递归查找 arr[lo..mid-1]
// int binary_search_recur(int arr[], int lo, int hi, int target) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int arr[] = {1, 3, 5, 7, 9, 11};
    int n = 6;
    int target = atoi(argv[1]);

    // TODO: 调用 binary_search_recur(arr, 0, n-1, target) 并打印结果
    // printf("%d\n", binary_search_recur(arr, 0, n - 1, target));
    (void)arr; (void)n; (void)target;

    return 0;
}
