// 45a_lcs_length.c — 计算最长公共子序列(LCS)的长度
//
// 任务: 用动态规划填表法计算两个字符串的 LCS 长度
//       从命令行读取两个字符串 argv[1] 和 argv[2]
//
// 知识点: 动态规划、二维数组、字符串比较
//
// 预期输出 (args=["ABCBDAB","BDCAB"]):
//   4

#include <stdio.h>
#include <string.h>

#define MAX_LEN 1024

int dp[MAX_LEN + 1][MAX_LEN + 1];

// TODO: 实现 lcs_length 函数
// 功能: 填充 dp 表，dp[i][j] 表示 s1[0..i-1] 和 s2[0..j-1] 的 LCS 长度
// 规则:
//   - 若 s1[i-1] == s2[j-1], 则 dp[i][j] = dp[i-1][j-1] + 1
//   - 否则 dp[i][j] = max(dp[i-1][j], dp[i][j-1])
// 返回: dp[m][n]，即 LCS 长度
//
// int lcs_length(const char *s1, const char *s2) { ... }

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <string1> <string2>\n", argv[0]);
        return 1;
    }

    // TODO: 调用 lcs_length(argv[1], argv[2]) 并打印结果
    // printf("%d\n", lcs_length(argv[1], argv[2]));

    return 0;
}
