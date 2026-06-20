/*
 * Lesson 42a: Lomuto 分区 — 参考答案
 */
#include <stdio.h>

void swap(int *a, int *b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int partition(int arr[], int lo, int hi)
{
    int pivot = arr[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[hi]);
    return i + 1;
}

int main(void)
{
    int arr[] = {4, 2, 6, 1, 3, 5};
    int n = 6;

    int pivot_idx = partition(arr, 0, n - 1);

    for (int i = 0; i < n; i++)
        printf("%d%c", arr[i], i < n - 1 ? ' ' : '\n');
    printf("pivot index: %d\n", pivot_idx);

    return 0;
}
