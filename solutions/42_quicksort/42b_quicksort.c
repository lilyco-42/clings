/*
 * Lesson 42b: 完整快速排序 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

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

void quicksort(int arr[], int lo, int hi)
{
    if (lo >= hi) return;
    int p = partition(arr, lo, hi);
    quicksort(arr, lo, p - 1);
    quicksort(arr, p + 1, hi);
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int n = argc - 1;
    int arr[64];
    for (int i = 0; i < n; i++)
        arr[i] = atoi(argv[i + 1]);

    quicksort(arr, 0, n - 1);

    for (int i = 0; i < n; i++)
        printf("%d%c", arr[i], i < n - 1 ? ' ' : '\n');

    return 0;
}
