/*
 * Lesson 41d: 泛型二分查找 — 参考答案
 */
#include <stdio.h>
#include <string.h>

void *generic_bsearch(const void *key, const void *base,
                      int n, int size,
                      int (*cmp)(const void *, const void *))
{
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        const void *elem = (const char *)base + mid * size;
        int r = cmp(key, elem);
        if (r == 0) return (void *)elem;
        else if (r > 0) lo = mid + 1;
        else hi = mid - 1;
    }
    return NULL;
}

int cmp_str(const void *a, const void *b)
{
    return strcmp(*(const char **)a, *(const char **)b);
}

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    const char *words[] = {"apple", "banana", "cherry", "date", "fig"};
    int n = 5;
    const char *target = argv[1];

    void *result = generic_bsearch(&target, words, n, sizeof(const char *), cmp_str);
    if (result) {
        int index = (int)((const char **)result - words);
        printf("found at %d\n", index);
    } else {
        printf("not found\n");
    }

    return 0;
}
