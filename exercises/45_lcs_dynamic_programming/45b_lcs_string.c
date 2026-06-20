// 45b_lcs_string.c — 回溯 DP 表输出 LCS 字符串
//
// 任务: 在 45a 的基础上，回溯 dp 表得到实际的 LCS 字符串并打印
//       从命令行读取两个字符串 argv[1] 和 argv[2]
//
// 知识点: DP 回溯、字符串构造
//
// 预期输出 (args=["ABCBDAB","BDCAB"]):
//   BCAB

#include <stdio.h>
#include <string.h>

#define MAX_LEN 1024

int dp[MAX_LEN + 1][MAX_LEN + 1];

// TODO: 实现 lcs_length 函数（同 45a）
// int lcs_length(const char *s1, const char *s2) { ... }

// TODO: 实现 lcs_string 函数
// 功能: 从 dp[m][n] 回溯，输出 LCS 字符串到 buf
// 回溯规则:
//   - 若 s1[i-1] == s2[j-1]，该字符属于 LCS，i--, j--
//   - 否则，若 dp[i-1][j] >= dp[i][j-1]，往上走 i--
//   - 否则，往左走 j--
// 注意: 回溯得到的字符是逆序的，需要反转或从后向前填充
//
// void lcs_string(const char *s1, const char *s2, char *buf) { ... }

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <string1> <string2>\n", argv[0]);
        return 1;
    }

    // TODO: 调用 lcs_length 填表
    // TODO: 调用 lcs_string 获取 LCS 字符串
    // TODO: printf("%s\n", buf);

    return 0;
}
