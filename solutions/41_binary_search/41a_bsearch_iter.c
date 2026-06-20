/*
 * Lesson 41a: 迭代二分查找 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

int binary_search(int arr[], int n, int target)
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        else if (arr[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int arr[] = {1, 3, 5, 7, 9, 11};
    int n = 6;
    int target = atoi(argv[1]);

    printf("%d\n", binary_search(arr, n, target));
    return 0;
}
