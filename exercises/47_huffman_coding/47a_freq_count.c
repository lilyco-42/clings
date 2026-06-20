// 47a_freq_count.c — 字符频率统计
//
// 任务: 统计输入字符串中每个字符的出现次数
//       按 ASCII 顺序输出非零频率字符，格式 "c:n"，空格分隔
//
// 知识点: 数组计数、字符处理
//
// 预期输出 (args=["aabbbcccc"]):
//   a:2 b:3 c:4

#include <stdio.h>
#include <string.h>

#define MAX_CHARS 256

// TODO: 实现 main 函数
// 1. 从 argv[1] 读取字符串
// 2. 用 int freq[256] = {0} 统计每个字符出现次数
//    for (int i = 0; s[i]; i++) freq[(unsigned char)s[i]]++;
// 3. 遍历 0-255，输出 freq > 0 的字符
//    格式: "a:2 b:3 c:4\n"（空格分隔，末尾换行）

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    // TODO: 统计频率并输出

    return 0;
}
