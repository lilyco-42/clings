// 44c_kmp_search.c — 完整 KMP 字符串匹配
//
// 任务: 实现 KMP 算法，找到 pattern 在 text 中第一次出现的位置
//       如果未找到，输出 -1
//
// 知识点: KMP 算法、next 表应用、O(n+m) 时间复杂度
//
// 用法: ./44c_kmp_search <text> <pattern>
// 示例: ./44c_kmp_search ABABDABACDABABCABAB ABABCABAB → 输出 "9"
//       ./44c_kmp_search AAAAAB AAB                   → 输出 "3"

#include <stdio.h>
#include <string.h>

#define MAX_LEN 1024

int next_table[MAX_LEN];

// TODO: 实现构建 next 表的函数 (同 44b)
// void build_next(const char *pattern, int m) { ... }

// TODO: 实现 KMP 搜索函数
// 功能: 在 text 中用 KMP 算法查找 pattern 第一次出现的位置
//   1. 构建 next 表
//   2. i = 0 (text 指针), j = 0 (pattern 指针)
//   3. 当 i < n:
//      a. 如果 text[i] == pattern[j]: i++, j++
//      b. 如果 j == m: 返回 i - j (匹配起始位置)
//      c. 如果 i < n 且 text[i] != pattern[j]:
//         - 如果 j > 0: j = next[j - 1] (利用 next 表跳过)
//         - 否则: i++ (pattern 第一个字符就不匹配)
//   4. 返回 -1 (未找到)
//
// int kmp_search(const char *text, const char *pattern) { ... }

int main(int argc, char *argv[])
{
    if (argc < 3) return 1;

    // TODO: 调用 kmp_search 并打印结果
    // printf("%d\n", kmp_search(argv[1], argv[2]));
    (void)argv;

    return 0;
}
