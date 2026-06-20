// 41c_lower_bound.c — lower_bound: 第一个 >= target 的位置
//
// 任务: 在有序数组 [1,3,3,5,7,7,9] 中找到第一个 >= target 的元素下标
//       如果所有元素都 < target，返回数组长度 n
//
// 知识点: 二分查找变体、左边界查找
//
// 用法: ./41c_lower_bound <target>
// 示例: ./41c_lower_bound 3  → 输出 "1" (第一个3在下标1)
//       ./41c_lower_bound 6  → 输出 "4" (第一个>=6是7，在下标4)
//       ./41c_lower_bound 10 → 输出 "7" (所有元素都<10，返回n)

#include <stdio.h>
#include <stdlib.h>

// TODO: 实现 lower_bound 函数
// 功能: 返回第一个 >= target 的元素下标
//   1. lo = 0, hi = n (注意: hi 初始为 n，不是 n-1)
//   2. 当 lo < hi:
//      a. mid = lo + (hi - lo) / 2
//      b. 如果 arr[mid] < target: lo = mid + 1
//      c. 否则: hi = mid
//   3. 返回 lo
// int lower_bound(int arr[], int n, int target) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int arr[] = {1, 3, 3, 5, 7, 7, 9};
    int n = 7;
    int target = atoi(argv[1]);

    // TODO: 调用 lower_bound 并打印结果
    // printf("%d\n", lower_bound(arr, n, target));
    (void)arr; (void)n; (void)target;

    return 0;
}
