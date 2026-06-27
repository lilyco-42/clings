/* 45_realloc.c — realloc 动态扩容实验（参考解答） */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char line[256];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

    int cap = 2, size = 0, expansions = 0;
    int *arr = malloc(cap * sizeof(int));
    printf("initial: %p (cap=%d)\n", (void *)arr, cap);

    char *tok = strtok(line, " \n");
    while (tok) {
        int val = atoi(tok);
        if (size >= cap) {
            cap *= 2;
            uintptr_t old_addr = (uintptr_t)arr;
            int *tmp = realloc(arr, cap * sizeof(int));
            if (tmp) {
                int moved = ((uintptr_t)tmp != old_addr);
                printf("realloc: 0x%lx -> 0x%lx (cap=%d) %s\n", (unsigned long)old_addr, (unsigned long)(uintptr_t)tmp,
                       cap, moved ? "[moved]" : "[in-place]");
                arr = tmp;
                expansions++;
            } else {
                free(arr);
                return 1;
            }
        }
        arr[size++] = val;
        tok = strtok(NULL, " \n");
    }
    printf("%d expansions, final cap=%d\n", expansions, cap);
    printf("values:");
    for (int i = 0; i < size; i++) printf(" %d", arr[i]);
    printf("\n");
    free(arr);
    return 0;
}
