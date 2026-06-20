/*
 * Lesson 45b: LCS 回溯输出子序列 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define MAX_LEN 1024

int dp[MAX_LEN + 1][MAX_LEN + 1];

int lcs_length(const char *s1, const char *s2)
{
    int m = (int)strlen(s1);
    int n = (int)strlen(s2);

    for (int i = 0; i <= m; i++)
        dp[i][0] = 0;
    for (int j = 0; j <= n; j++)
        dp[0][j] = 0;

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1] + 1;
            else
                dp[i][j] = dp[i - 1][j] > dp[i][j - 1] ? dp[i - 1][j] : dp[i][j - 1];
        }
    }

    return dp[m][n];
}

void lcs_string(const char *s1, const char *s2, char *buf)
{
    int m = (int)strlen(s1);
    int n = (int)strlen(s2);
    int len = dp[m][n];
    buf[len] = '\0';

    int i = m, j = n, k = len - 1;
    while (i > 0 && j > 0) {
        if (s1[i - 1] == s2[j - 1]) {
            buf[k] = s1[i - 1];
            i--;
            j--;
            k--;
        } else if (dp[i - 1][j] >= dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <string1> <string2>\n", argv[0]);
        return 1;
    }

    lcs_length(argv[1], argv[2]);

    char buf[MAX_LEN + 1];
    lcs_string(argv[1], argv[2], buf);
    printf("%s\n", buf);

    return 0;
}
