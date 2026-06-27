/* 44_qsort.c — qsort 泛型排序：int 与字符串数组
 *
 * 任务: 调用 qsort 分别完成两组排序
 *       1. 实现 cmp_str — 字符串比较器
 *       2. int 数组排序: 使用给定的 cmp_int
 *       3. 字符串数组排序: 使用你自己写的 cmp_str
 *
 * 核心知识点: 为什么字符串数组的 cmp 参数要强转为 *(const char **)a？
 *   qsort 传给 cmp 的参数是"数组中元素的地址"
 *   对于 int 数组 arr[10]: 元素是 int → 地址是 int* → cmp 收到 int*
 *   对于字符串数组 strs[10]: 元素是 char* → 地址是 char** → cmp 收到 char**
 *   所以 cmp_str 需要二级指针解引用！
 *
 * 验证：
 *   stdin: "int 5 3 8 1 4\n"          → ints: 1 3 4 5 8
 *   stdin: "str cherry apple banana\n" → strings: apple banana cherry
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmp_int(const void *a, const void *b) { return *(const int *)a - *(const int *)b; }

/* 字符串比较器 — 注意二级指针！ */
int cmp_str(const void *a, const void *b) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 先把 void* 强转为 const char**，再解引用得到 const char*
     * sa = *(const char **)a
     * sb = *(const char **)b */

    /* strcmp(sa, sb) 比较两个字符串 */
}

int main(void) {
    char line[512];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

    /* 第一个 token 是模式: "int" 或 "str" */
    char *mode = strtok(line, " ");

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 若 mode == "int":
     *   解析后续数字到 int arr[64]
     *   qsort(arr, n, sizeof(int), cmp_int)
     *   打印 "ints: N1 N2 ..."
     *
     * 若 mode == "str":
     *   将后续每个 token 的地址存入 char *strs[64]（不需要拷贝！直接用 token 指针）
     *   qsort(strs, n, sizeof(char *), cmp_str)
     *   打印 "strings: S1 S2 ..." */

    return 0;
}
