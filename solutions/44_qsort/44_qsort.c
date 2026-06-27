/* 44_qsort.c — qsort 泛型排序（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmp_int(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }
int cmp_str(const void *a, const void *b) {
    const char *sa = *(const char **)a;
    const char *sb = *(const char **)b;
    return strcmp(sa, sb);
}

int main(void) {
    char line[512];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
    char *mode = strtok(line, " ");
    if (strcmp(mode, "int") == 0) {
        int arr[64], n = 0;
        char *tok;
        while ((tok = strtok(NULL, " "))) arr[n++] = atoi(tok);
        qsort(arr, n, sizeof(int), cmp_int);
        printf("ints:");
        for (int i = 0; i < n; i++) printf(" %d", arr[i]);
        printf("\n");
    } else {
        char *strs[64];
        int n = 0;
        char *tok;
        while ((tok = strtok(NULL, " "))) strs[n++] = tok;
        qsort(strs, n, sizeof(char *), cmp_str);
        printf("strings:");
        for (int i = 0; i < n; i++) printf(" %s", strs[i]);
        printf("\n");
    }
    return 0;
}
