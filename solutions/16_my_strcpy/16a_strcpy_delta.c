#include <stdio.h>

/*
 * delta 优化版：只维护一个指针 s，通过偏移 delt 写入 dst
 * 出处：林锐《高质量程序设计指南》
 *
 * 注意：为保证 delt = dst - src 合法 (指向同一数组内)，
 * main 中将 src 和 dst 放在同一个 buf 中的不同偏移
 */
char *mystrcpy_delt(char *dst, const char *src) {
    char *s = (char *)src;
    int delt = dst - src;

    while (*s != '\0') {
        s[delt] = *s;
        s++;
    }
    s[delt] = '\0';

    return dst;
}

/* buf 保证 src 和 dst 在同一数组内，delt 指针运算合法 */
char buf[512];

int main(void) {
    char *src = buf;
    char *dst = buf + 256;

    fgets(src, 256, stdin);
    int i = 0;
    while (src[i] && src[i] != '\n') i++;
    src[i] = '\0';

    mystrcpy_delt(dst, src);
    printf("%s\n", dst);

    return 0;
}
