/* 42_snprintf.c — snprintf 格式化安全（参考解答） */
#include <stdio.h>
#include <string.h>

int is_truncated(char *buf, int size, int n, int *ret) {
    *ret = snprintf(buf, size, "Count: %d", n);
    return (*ret >= size);
}

int main(void) {
    char line[32];
    fgets(line, sizeof(line), stdin);
    int n, ret, trunc;
    sscanf(line, "%d", &n);
    char buf[10];
    trunc = is_truncated(buf, 10, n, &ret);
    printf("buf: '%s'\n", buf);
    printf("len: %zu\n", strlen(buf));
    printf("ret: %d\n", ret);
    printf("truncated: %s\n", trunc ? "yes" : "no");
    return 0;
}
