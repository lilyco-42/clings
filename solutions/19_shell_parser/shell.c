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

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }
int mydiv(int a, int b) {
    if (b != 0) return a / b;
    return 0;
}

int power(int a, int b) {
    int result = 1;
    int i;
    for (i = 0; i < b; i++) result *= a;
    return result;
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

struct operation {
    char name[8];
    int (*pf)(int, int);
    char opchar;
} op[] = {{"add", add, '+'}, {"sub", sub, '-'}, {"mul", mul, 'x'}, {"div", mydiv, '/'}, {"p", power, '^'}};

int command_do(int argc, char *argv[]) {
    int i = 0;

    for (i = 0; i < (int)(sizeof(op) / sizeof(op[0])); i++) {
        if (strcmp(argv[0], op[i].name) == 0) {
            pf = op[i].pf;
            opchar = op[i].opchar;
            break;
        }
    }

    math_main(argc, argv);

    return 0;
}

int main(void) {
    char buf[256];
    int argc;
    char *argv[10];

    fgets(buf, sizeof(buf), stdin);
    argc = shell_parse(buf, argv);
    command_do(argc, argv);

    return 0;
}
