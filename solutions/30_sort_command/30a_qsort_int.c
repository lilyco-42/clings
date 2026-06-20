/*
 * Lesson 30a: qsort 整数排序 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>

static int cmp_int(const void *a, const void *b)
{
    int va = *(const int *)a;
    int vb = *(const int *)b;
    return (va > vb) - (va < vb);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <num1> <num2> ...\n", argv[0]);
        return 1;
    }

    int n = argc - 1;
    int *arr = malloc(n * sizeof(int));
    if (arr == NULL) {
        fprintf(stderr, "malloc failed\n");
        return 1;
    }

    for (int i = 0; i < n; i++)
        arr[i] = atoi(argv[i + 1]);

    qsort(arr, n, sizeof(int), cmp_int);

    for (int i = 0; i < n; i++)
        printf("%d\n", arr[i]);

    free(arr);
    return 0;
}
