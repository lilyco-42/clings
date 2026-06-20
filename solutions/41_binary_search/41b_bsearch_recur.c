/*
 * Lesson 41b: 递归二分查找 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

int binary_search_recur(int arr[], int lo, int hi, int target)
{
    if (lo > hi) return -1;
    int mid = lo + (hi - lo) / 2;
    if (arr[mid] == target) return mid;
    if (arr[mid] < target)
        return binary_search_recur(arr, mid + 1, hi, target);
    return binary_search_recur(arr, lo, mid - 1, target);
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int arr[] = {1, 3, 5, 7, 9, 11};
    int n = 6;
    int target = atoi(argv[1]);

    printf("%d\n", binary_search_recur(arr, 0, n - 1, target));
    return 0;
}
