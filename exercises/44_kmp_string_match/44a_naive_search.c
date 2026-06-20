// 44a_naive_search.c — 朴素字符串匹配 O(n*m)
//
// 任务: 实现暴力字符串匹配，输出所有匹配位置
//       如果没有匹配，输出 -1
//
// 知识点: 双重循环、逐位比较
//
// 用法: ./44a_naive_search <text> <pattern>
// 示例: ./44a_naive_search ABABCABAB ABAB → 输出 "0 5"
//       ./44a_naive_search hello xyz    → 输出 "-1"

#include <stdio.h>
#include <string.h>

// TODO: 实现朴素字符串匹配函数
// 功能: 在 text 中查找 pattern 的所有出现位置，打印位置（空格分隔）
//       如果没有找到任何匹配，打印 -1
//
// 算法:
//   int n = strlen(text), m = strlen(pattern);
//   int found = 0;
//   for (int i = 0; i <= n - m; i++) {
//       int j;
//       for (j = 0; j < m; j++) {
//           if (text[i + j] != pattern[j]) break;
//       }
//       if (j == m) {  // 完全匹配
//           if (found > 0) printf(" ");
//           printf("%d", i);
//           found++;
//       }
//   }
//   if (found == 0) printf("-1");
//   printf("\n");
//
// void naive_search(const char *text, const char *pattern) { ... }

int main(int argc, char *argv[])
{
    if (argc < 3) return 1;

    // TODO: 调用 naive_search(argv[1], argv[2])
    (void)argv;

    return 0;
}
