/* 47_setjmp_longjmp.c — setjmp/longjmp 非局部跳转（参考解答） */
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

jmp_buf env;

void funcC(const char *input) {
    if (strcmp(input, "error") == 0) longjmp(env, 1);
    printf("funcC: %s\n", input);
}
void funcB(const char *input) {
    funcC(input);
    printf("funcB done\n");
}
void funcA(const char *input) {
    funcB(input);
    printf("funcA done\n");
}

int main(void) {
    char line[64];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    int r = setjmp(env);
    if (r == 0) {
        funcA(line);
        printf("success: %s\n", line);
    } else {
        printf("caught error: %s\n", line);
    }
    return 0;
}
