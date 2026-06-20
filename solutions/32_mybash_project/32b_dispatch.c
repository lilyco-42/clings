/*
 * Lesson 32b: 函数指针分发表 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ARGS 32

static int cmd_hello(int argc, char *argv[])
{
    (void)argc; (void)argv;
    printf("Hello, NCCL!\n");
    return 0;
}

static int cmd_add(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: add <a> <b>\n");
        return 1;
    }
    printf("%d\n", atoi(argv[1]) + atoi(argv[2]));
    return 0;
}

static int cmd_echo(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++) {
        if (i > 1) printf(" ");
        printf("%s", argv[i]);
    }
    printf("\n");
    return 0;
}

struct operation {
    char name[16];
    int (*pf)(int, char **);
};

static struct operation op[] = {
    { "hello", cmd_hello },
    { "add",   cmd_add },
    { "echo",  cmd_echo },
};

static int command_do(int argc, char *argv[])
{
    if (argc < 1 || argv[0] == NULL)
        return -1;

    for (int i = 0; i < (int)(sizeof(op) / sizeof(op[0])); i++) {
        if (strcmp(argv[0], op[i].name) == 0)
            return op[i].pf(argc, argv);
    }

    fprintf(stderr, "command not found: %s\n", argv[0]);
    return -1;
}

static void shell_parse(char *buf)
{
    int argc = 0;
    char *argv[MAX_ARGS];
    int in_word = 0;

    while (*buf) {
        if (*buf == '\n') {
            *buf = '\0';
            break;
        }
        if (*buf == ' ' || *buf == '\t') {
            if (in_word) {
                *buf = '\0';
                in_word = 0;
            }
        } else {
            if (!in_word && argc < MAX_ARGS - 1) {
                argv[argc++] = buf;
                in_word = 1;
            }
        }
        buf++;
    }
    argv[argc] = NULL;

    if (argc > 0)
        command_do(argc, argv);
}

int main(void)
{
    char buf[256];

    if (fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;

    shell_parse(buf);
    return 0;
}
