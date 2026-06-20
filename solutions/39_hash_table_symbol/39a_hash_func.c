/*
 * Lesson 39a: djb2 哈希函数 — 参考答案
 */
#include <stdio.h>

unsigned long djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = hash * 33 + c;
    return hash;
}

int main(void)
{
    const char *words[] = {"apple", "banana", "cherry"};
    int n = 3;

    for (int i = 0; i < n; i++)
        printf("%s -> %lu\n", words[i], djb2(words[i]) % 1024);

    return 0;
}
