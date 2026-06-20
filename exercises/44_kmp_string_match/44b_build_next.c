// 44b_build_next.c — 构建 KMP next/failure 表
//
// 任务: 对给定模式串构建 KMP 的 next 数组（前缀函数/失败函数）
//       next[i] = pattern[0..i] 的最长真前缀等于后缀的长度
//
// 知识点: KMP 前缀函数、双指针构造
//
// 用法: ./44b_build_next <pattern>
// 示例: ./44b_build_next ABABC   → 输出 "0 0 1 2 0"
//       ./44b_build_next AABAAAB → 输出 "0 1 0 1 2 2 3"

#include <stdio.h>
#include <string.h>

#define MAX_LEN 256

int next_table[MAX_LEN];

// TODO: 实现构建 next 表的函数
// 功能: 计算 pattern 的前缀函数
//   1. next[0] = 0（单个字符没有真前缀）
//   2. len = 0 (当前匹配的前缀长度)
//   3. i 从 1 到 m-1:
//      a. 当 len > 0 且 pattern[i] != pattern[len]:
//         len = next[len - 1] (回退)
//      b. 如果 pattern[i] == pattern[len]:
//         len++
//      c. next[i] = len
//
// void build_next(const char *pattern, int m) { ... }

int main(int argc, char *argv[])
{
    if (argc < 2) return 1;

    const char *pattern = argv[1];
    int m = (int)strlen(pattern);

    // TODO: 调用 build_next(pattern, m)
    // TODO: 打印 next 表（空格分隔，末尾换行）
    // for (int i = 0; i < m; i++)
    //     printf("%d%c", next_table[i], i < m-1 ? ' ' : '\n');
    (void)pattern; (void)m;

    return 0;
}
