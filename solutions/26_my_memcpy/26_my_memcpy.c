/* 26_my_memcpy.c — 实现 my_memcpy：标准内存拷贝（参考解答） */
#include <stdio.h>
#include <string.h>

void *my_memcpy(void *dest, const void *src, size_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    for (size_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

int main(void) {
    char src[256], dest[256] = {0};
    fgets(src, sizeof(src), stdin);
    int i = 0;
    while (src[i] && src[i] != '\n') i++;
    src[i] = '\0';

    my_memcpy(dest, src, strlen(src) + 1);
    printf("%s\n", dest);
    return 0;
}
