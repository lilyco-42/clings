#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int (*pf)(int, int);
char opchar;

int shell_parse(char *buf, char *argv[]) {
    int argc = 0;
    int state = 0;

    while (*buf && *buf != '\n') {
        if (*buf != ' ' && state == 0) {
            argv[argc++] = buf;
            state = 1;
        }
        if (*buf == ' ' && state == 1) {
            *buf = '\0';
            state = 0;
        }
        buf++;
    }
    if (*buf == '\n') *buf = '\0';
    return argc;
}

int math_main(int argc, char *argv[]) {
    int a, b;
    int result;

    if (argc < 3) return -1;

    a = atoi(argv[1]);
    b = atoi(argv[2]);

    result = pf(a, b);

    printf("result: %s %c %s = %d\n", argv[1], opchar, argv[2], result);

    return 0;
}

int add(int a, int b) { return a + b; }

int sub(int a, int b) { return a - b; }

int main(void) {
    char buf[256];
    int argc;
    char *argv[10];

    fgets(buf, sizeof(buf), stdin);
    argc = shell_parse(buf, argv);

    if (strcmp(argv[0], "add") == 0) {
        pf = add;
        opchar = '+';
    }
    if (strcmp(argv[0], "sub") == 0) {
        pf = sub;
        opchar = '-';
    }

    math_main(argc, argv);

    return 0;
}
