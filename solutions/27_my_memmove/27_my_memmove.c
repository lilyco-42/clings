/* 27_my_memmove.c — 实现 my_memmove：内存重叠拷贝（参考解答） */
#include <stdio.h>
#include <string.h>

void *my_memmove(void *dest, const void *src, size_t n) {
    char *d = (char *)dest;
    const char *s = (const char *)src;
    if (d == s || n == 0) return dest;
    if (d < s) /* dest 在 src 前面: 正向拷贝 */
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    else /* dest 在 src 后面: 反向拷贝，从末尾开始 */
        for (size_t i = n; i > 0; i--) d[i - 1] = s[i - 1];
    /* d == s 时无需拷贝 */
    return dest;
}

int main(void) {
    char buf[256];
    fgets(buf, sizeof(buf), stdin);
    int i = 0;
    while (buf[i] && buf[i] != '\n') i++;
    buf[i] = '\0';
    size_t len = strlen(buf);
    if (len > 0) my_memmove(buf + 1, buf, len - 1);
    printf("%s\n", buf);
    return 0;
}
