/* 41_quick_sort.c — 快速排序（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

int partition(int arr[], int lo, int hi) {
    int pivot = arr[hi], i = lo - 1;
    for (int j = lo; j < hi; j++)
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    swap(&arr[i + 1], &arr[hi]);
    return i + 1;
}
void quicksort(int arr[], int lo, int hi) {
    if (lo >= hi) return;
    int p = partition(arr, lo, hi);
    quicksort(arr, lo, p - 1);
    quicksort(arr, p + 1, hi);
}

int main(void) {
    char line[1024];
    fgets(line, sizeof(line), stdin);
    int arr[1024], n = 0;
    char *tok = strtok(line, " \n");
    while (tok) {
        arr[n++] = atoi(tok);
        tok = strtok(NULL, " \n");
    }
    quicksort(arr, 0, n - 1);
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%d", arr[i]);
    }
    printf("\n");
    return 0;
}
