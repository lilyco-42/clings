/* 25_my_strstr.c — 实现 my_strstr：暴力字符串匹配
 *
 * 任务: 实现 my_strstr(haystack, needle)
 *       在 haystack 中查找 needle 第一次出现的位置
 *       返回指向该位置的指针；若未找到返回 NULL
 *       如果 needle 是空字符串，返回 haystack
 *
 * 知识点: 双重循环、滑动窗口、边界条件（<= n-m）、空串处理
 *         为后续 KMP 算法打底
 *
 * 验证：
 *   stdin: "hello world\nworld\n"  → found: world
 *   stdin: "hello world\nxyz\n"    → not found
 *   stdin: "aaaa\naa\n"            → found: aaaa
 *   stdin: "hello\n\n"             → found: hello  （空 needle）
 */
#include <stdio.h>
#include <string.h>

char *my_strstr(const char *haystack, const char *needle) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 如果 needle 是空串，直接返回 haystack */

    /* 计算两个字符串的长度 */

    /* 外层循环：i 从 0 到 n-m。为什么必须用 <= 而不是 < ？

     * （提示：当模式串刚好在末尾时，i = n-m 正好对齐） */

    /* 内层循环：逐字符比较 haystack[i+j] 和 needle[j]，不匹配则 break */

    /* 如果内层循环完整跑完（j == m），说明从 i 位置完全匹配 */
}

int main(void) {
    char haystack[256], needle[256];
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 用 fgets 从 stdin 读两行：haystack 和 needle */

    /* 去掉每行末尾的 '\n'（遍历到 '\n' 替换为 '\0'） */
    /* 注意：空行（直接回车）读到的是 "\n"，去掉后变成空串 */

    /* 调用 my_strstr，若返回值非 NULL 则打印 "found: " + 返回的字符串，
     * 否则打印 "not found" */
    return 0;
}
