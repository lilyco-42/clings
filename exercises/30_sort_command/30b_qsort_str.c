// 30b_qsort_str.c — 用 qsort 对字符串数组排序
//
// 任务: 从命令行参数读取字符串，用 qsort 排序后输出。
//
// 知识点: qsort 对 char* 数组排序时的二级指针转换
//         const void* → const char** → 解引用得到 const char*
//
// 用法: ./30b_qsort_str red black blue green yellow
// 输出: 每行一个字符串，按字典序升序

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: 实现 cmp_str(const void *a, const void *b)
//       关键: 参数是指向 char* 的指针（因为数组元素是 char*）
//       所以需要: const char *s1 = *(const char **)a;
//       然后用 strcmp(s1, s2) 比较

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <str1> <str2> ...\n", argv[0]);
        return 1;
    }

    // TODO: 对 argv+1 进行排序（共 argc-1 个元素，每个是 char*）
    //       qsort(argv + 1, argc - 1, sizeof(char *), cmp_str);
    // TODO: 循环打印 argv[i+1]，格式 "%s\n"

    return 0;
}
