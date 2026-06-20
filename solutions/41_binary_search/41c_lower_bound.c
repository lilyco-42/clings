/*
 * Lesson 41c: lower_bound 左边界查找 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

int lower_bound(int arr[], int n, int target)
{
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] < target)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int arr[] = {1, 3, 3, 5, 7, 7, 9};
    int n = 7;
    int target = atoi(argv[1]);

    printf("%d\n", lower_bound(arr, n, target));
    return 0;
}
