/*
 * Lesson 32a: 命令行字符串解析 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define MAX_ARGS 32

int main(void)
{
    char buf[256];

    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;

    int argc = 0;
    char *argv[MAX_ARGS];
    int in_word = 0;
    char *p = buf;

    while (*p) {
        if (*p == '\n') {
            *p = '\0';
            break;
        }
        if (*p == ' ' || *p == '\t') {
            if (in_word) {
                *p = '\0';
                in_word = 0;
            }
        } else {
            if (!in_word && argc < MAX_ARGS - 1) {
                argv[argc++] = p;
                in_word = 1;
            }
        }
        p++;
    }
    argv[argc] = NULL;

    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++)
        printf("argv[%d]: %s\n", i, argv[i]);

    return 0;
}
