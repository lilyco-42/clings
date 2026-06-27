/* 46_sscanf.c — sscanf 高级格式化解析（参考解答） */
#include <stdio.h>
#include <string.h>

int parse_addr(const char *input, char *host, int *port) {
    int n = sscanf(input, "%[^:]:%d", host, port);
    return (n == 2);
}

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    char host[128];
    int port;
    if (parse_addr(line, host, &port)) {
        printf("host: %s\n", host);
        printf("port: %d\n", port);
    } else {
        printf("parse error\n");
    }
    return 0;
}
