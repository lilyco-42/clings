// 39a_hash_func.c — 实现 djb2 哈希函数
//
// 任务: 实现经典的 djb2 哈希函数
//       对 "apple","banana","cherry" 计算 hash % 1024 并打印
//
// 知识点: djb2 算法
//   hash = 5381
//   对每个字符 c: hash = hash * 33 + c
//   使用 unsigned long 避免溢出问题
//
// 预期输出:
//   apple -> 567
//   banana -> 38
//   cherry -> 466

#include <stdio.h>

// TODO: 实现 djb2 哈希函数
// 功能:
//   unsigned long hash = 5381;
//   对字符串中每个字符 c:
//     hash = hash * 33 + c;
//   返回 hash
// unsigned long djb2(const char *str) { ... }

int main(void)
{
    const char *words[] = {"apple", "banana", "cherry"};
    int n = 3;

    // TODO: 对每个单词计算 djb2(word) % 1024
    // TODO: printf("%s -> %lu\n", words[i], djb2(words[i]) % 1024);
    (void)words;
    (void)n;

    return 0;
}
