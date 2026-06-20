/*
 * Lesson 42c: 泛型快速排序 — 参考答案
 */
#include <stdio.h>
#include <string.h>

void generic_swap(void *a, void *b, int size)
{
    char tmp[256];
    memcpy(tmp, a, (size_t)size);
    memcpy(a, b, (size_t)size);
    memcpy(b, tmp, (size_t)size);
}

int generic_partition(void *base, int lo, int hi, int size,
                      int (*cmp)(const void *, const void *))
{
    char *arr = (char *)base;
    char *pivot = arr + hi * size;
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (cmp(arr + j * size, pivot) <= 0) {
            i++;
            generic_swap(arr + i * size, arr + j * size, size);
        }
    }
    generic_swap(arr + (i + 1) * size, arr + hi * size, size);
    return i + 1;
}

void generic_qsort(void *base, int lo, int hi, int size,
                   int (*cmp)(const void *, const void *))
{
    if (lo >= hi) return;
    int p = generic_partition(base, lo, hi, size, cmp);
    generic_qsort(base, lo, p - 1, size, cmp);
    generic_qsort(base, p + 1, hi, size, cmp);
}

int cmp_str(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    int n = argc - 1;
    const char *words[64];
    for (int i = 0; i < n; i++)
        words[i] = argv[i + 1];

    generic_qsort(words, 0, n - 1, sizeof(const char *), cmp_str);

    for (int i = 0; i < n; i++)
        printf("%s%c", words[i], i < n - 1 ? ' ' : '\n');

    return 0;
}
