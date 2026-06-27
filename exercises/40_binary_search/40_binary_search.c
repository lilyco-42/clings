/* 40_binary_search.c — 二分查找
 *
 * 任务: 实现迭代二分查找 binary_search(arr, n, target)
 *       在有序数组中查找 target，返回索引；未找到返回 -1
 *       重点考察边界条件: mid = lo+(hi-lo)/2 防溢出, while(lo<=hi)
 *
 * 知识点: 循环不变式、防溢出 mid（不使用 (lo+hi)/2）、边界终止条件
 *
 * 验证：
 *   stdin: "1 3 5 7 9 11\n5\n" → 2
 *   stdin: "1 3 5 7 9 11\n6\n" → -1
 *   stdin: "1\n1\n"            → 0
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 迭代二分查找：在大小为 n 的有序数组 arr 中查找 target */
int binary_search(int arr[], int n, int target) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 初始化搜索范围：lo = 0, hi = n - 1 */

    /* while (lo <= hi) — 等号很重要！考虑只有1个元素时的情况 */

    /* 计算中点：mid = lo + (hi - lo) / 2
     * 为什么不用 (lo + hi) / 2？→ 避免整数溢出 */

    /* 比较 arr[mid] 和 target：
     *   等于 → 找到了，返回 mid
     *   小于 → target 在右边，lo = mid + 1
     *   大于 → target 在左边，hi = mid - 1 */

    /* 循环结束仍未找到 → 返回 -1 */
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
