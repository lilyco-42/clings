/* 41_quick_sort.c — 快速排序
 *
 * 任务: 实现快速排序
 *       1. partition(arr, lo, hi) — Lomuto 分区
 *          选 arr[hi] 为 pivot，扫描 [lo, hi) 区间，小的放左边
 *          返回 pivot 最终位置
 *       2. quicksort(arr, lo, hi) — 递归快排
 *          分区 → 递归排左半 → 递归排右半
 *
 * 知识点: 分治、Lomuto 分区、pivot 选择、原地排序
 *
 * 验证：
 *   stdin: "5 3 8 1 4 2\n" → 1 2 3 4 5 8
 *   stdin: "9 1 5 3 7\n"   → 1 3 5 7 9
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/* Lomuto 分区：
 * - 选 arr[hi] 作为 pivot
 * - i 指向"小于 pivot 的区域"的最后一个位置
 * - j 扫描 [lo, hi)，遇到 ≤ pivot 的元素就交换到左边
 * - 最后把 pivot 放到正确位置 (i+1) */
int partition(int arr[], int lo, int hi) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 选 pivot = arr[hi]; i = lo - 1 */

    /* for j = lo; j < hi; j++：
     *   如果 arr[j] <= pivot：
     *      i++; swap(&arr[i], &arr[j]) */

    /* 把 pivot 放到中间：swap(&arr[i+1], &arr[hi]); return i+1 */
}

/* 递归快排 */
void quicksort(int arr[], int lo, int hi) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 终止条件：lo >= hi → return */

    /* 分区，得到 pivot 位置 p = partition(arr, lo, hi) */

    /* 递归排左半：quicksort(arr, lo, p-1) */

    /* 递归排右半：quicksort(arr, p+1, hi) */
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
