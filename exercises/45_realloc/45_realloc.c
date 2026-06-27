/* 45_realloc.c — realloc 动态扩容实验
 *
 * 任务: 用 malloc 分配初始内存，循环 realloc 扩容
 *       1. 每次扩容前后打印指针地址（判断 in-place vs moved）
 *       2. 掌握安全用法：为什么不能写 ptr = realloc(ptr, size)
 *
 * realloc 两种行为:
 *   原地扩容 [in-place]: 当前内存后有足够空间 → 返回原指针
 *   异地搬迁 [moved]: 空间不足 → 分配新内存 + memcpy + free 旧内存
 *
 * 致命错误: ptr = realloc(ptr, size);
 *   → 若 realloc 失败返回 NULL → ptr = NULL → 原内存泄漏！
 *   正确写法: tmp = realloc(ptr, size); if (tmp) ptr = tmp;
 *
 * 知识点: realloc 安全用法、原地 vs 异地、uintptr_t 防 use-after-free 警告
 *
 * 验证：
 *   stdin: "1 2 3 4 5\n" → 显示扩容过程 + values: 1 2 3 4 5
 *   stdin: "10 20\n"     → values: 10 20（无扩容）
 */
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

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 用 strtok 遍历 line 中的数字：
     *   for each token:
     *     val = atoi(token)
     *
     *     if (size >= cap):   // 需要扩容
     *       cap *= 2
     *       保存旧地址（uintptr_t old = (uintptr_t)arr）
     *       安全 realloc: int *tmp = realloc(arr, cap*sizeof(int))
     *       if (tmp) {
     *         判断是否 moved: ((uintptr_t)tmp != old)
     *         打印扩容详情（地址+容量+moved/in-place）
     *         arr = tmp; expansions++
     *       } else {
     *         free(arr); return 1;  // realloc 失败
     *       }
     *
     *     arr[size++] = val */

    /* 打印总结 */
    printf("%d expansions, final cap=%d\n", expansions, cap);
    printf("values:");
    for (int i = 0; i < size; i++) printf(" %d", arr[i]);
    printf("\n");
    free(arr);
    return 0;
}
