/* 40_binary_search.c — 二分查找（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int binary_search(int arr[], int n, int target) {
    int lo = 0, hi = n - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid] == target) return mid;
        if (arr[mid] < target)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return -1;
}

int main(void) {
    char line[1024], tline[32];
    fgets(line, sizeof(line), stdin);
    fgets(tline, sizeof(tline), stdin);
    int arr[1024], n = 0, target;
    char *tok = strtok(line, " \n");
    while (tok) {
        arr[n++] = atoi(tok);
        tok = strtok(NULL, " \n");
    }
    sscanf(tline, "%d", &target);
    printf("%d\n", binary_search(arr, n, target));
    return 0;
}
