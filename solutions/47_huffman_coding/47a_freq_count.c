/*
 * Lesson 47a: 字符频率统计 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define MAX_CHARS 256

struct char_freq {
    char ch;
    int freq;
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    const char *s = argv[1];
    int freq[MAX_CHARS] = {0};

    for (int i = 0; s[i]; i++)
        freq[(unsigned char)s[i]]++;

    /* 收集非零频率的字符 */
    struct char_freq result[MAX_CHARS];
    int count = 0;

    for (int c = 0; c < MAX_CHARS; c++) {
        if (freq[c] > 0) {
            result[count].ch = (char)c;
            result[count].freq = freq[c];
            count++;
        }
    }

    /* 按字符 ASCII 排序（已经是按序遍历的，无需额外排序） */
    for (int i = 0; i < count; i++) {
        if (i > 0) printf(" ");
        printf("%c:%d", result[i].ch, result[i].freq);
    }
    printf("\n");

    return 0;
}
