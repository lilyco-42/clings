/* 43_strtok_r.c — strtok_r 线程安全字符串切分（参考解答） */
#include <stdio.h>
#include <string.h>

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

    char *argv[64], *saveptr;
    int argc = 0;
    char *tok = strtok_r(line, " \t", &saveptr);
    while (tok) {
        argv[argc++] = tok;
        tok = strtok_r(NULL, " \t", &saveptr);
    }
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) printf("argv[%d]: %s\n", i, argv[i]);
    return 0;
}
