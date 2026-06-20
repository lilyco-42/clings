// 45c_diff_simple.c — 基于 LCS 的简单 diff
//
// 任务: 利用 LCS 找出两个字符串的差异
//       s1 中不在 LCS 里的字符标记为 "- X"（被删除）
//       s2 中不在 LCS 里的字符标记为 "+ X"（被添加）
//       从命令行读取两个字符串 argv[1] 和 argv[2]
//
// 知识点: LCS 应用、diff 算法基础
//
// 预期输出 (args=["ABCDEF","ABDEG"]):
//   - C
//   - F
//   + G

#include <stdio.h>
#include <string.h>

#define MAX_LEN 1024

int dp[MAX_LEN + 1][MAX_LEN + 1];
char lcs_buf[MAX_LEN + 1];

// TODO: 实现 lcs_length 函数（同 45a）
// int lcs_length(const char *s1, const char *s2) { ... }

// TODO: 实现 lcs_string 函数（同 45b）
// void lcs_string(const char *s1, const char *s2, char *buf) { ... }

// TODO: 实现 print_diff 函数
// 功能: 先输出 s1 中被删除的字符（不在 LCS 中），再输出 s2 中新增的字符
// 算法:
//   用指针 i 遍历 s1，指针 k 遍历 lcs:
//     若 s1[i] == lcs[k]，两者同时前进
//     否则打印 "- %c\n" 并只移动 i
//   类似地，用指针 j 遍历 s2，指针 k 遍历 lcs:
//     若 s2[j] == lcs[k]，两者同时前进
//     否则打印 "+ %c\n" 并只移动 j
//
// void print_diff(const char *s1, const char *s2, const char *lcs) { ... }

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <string1> <string2>\n", argv[0]);
        return 1;
    }

    // TODO: 计算 LCS 字符串
    // TODO: 调用 print_diff 输出差异

    return 0;
}
