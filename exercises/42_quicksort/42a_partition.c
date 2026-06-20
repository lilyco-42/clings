// 42a_partition.c — Lomuto 分区
//
// 任务: 对数组 [4,2,6,1,3,5] 执行 Lomuto 分区（以最后一个元素为 pivot）
//       打印分区后的数组和 pivot 最终位置
//
// 知识点: Lomuto 分区方案、pivot 选择、swap 操作
//
// 预期输出:
//   4 2 1 3 5 6
//   pivot index: 4

#include <stdio.h>

// TODO: 实现 swap 函数
// void swap(int *a, int *b) { ... }

// TODO: 实现 Lomuto 分区函数
// 功能: 以 arr[hi] 为 pivot，将 <= pivot 的放左边，> pivot 的放右边
//   1. pivot = arr[hi]
//   2. i = lo - 1 (i 指向"小于等于区"的末尾)
//   3. for j = lo to hi-1:
//      如果 arr[j] <= pivot: i++, swap(arr[i], arr[j])
//   4. swap(arr[i+1], arr[hi]) — 把 pivot 放到正确位置
//   5. 返回 i+1 (pivot 的最终下标)
// int partition(int arr[], int lo, int hi) { ... }

int main(void)
{
    int arr[] = {4, 2, 6, 1, 3, 5};
    int n = 6;

    // TODO: 调用 partition(arr, 0, n-1)
    // TODO: 打印分区后的数组（空格分隔）
    // TODO: 打印 "pivot index: %d\n"
    (void)arr; (void)n;

    return 0;
}
