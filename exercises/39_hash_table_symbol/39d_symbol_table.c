// 39d_symbol_table.c — 符号表: 用哈希表统计单词频率
//
// 任务: 从 stdin 读入一行，按空格分词
//       使用哈希表统计每个单词出现次数
//       按字典序输出 "单词:次数"（用辅助排序）
//
// 知识点: 哈希表 + 计数器、收集所有 key 后排序输出
//
// 示例: 输入 "the cat sat on the mat"
//       输出 "cat:1 mat:1 on:1 sat:1 the:2"
//
// 预期输出（stdin="the cat sat on the mat\n"）:
//   cat:1 mat:1 on:1 sat:1 the:2

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 256
#define MAX_KEY 64
#define MAX_LINE 1024

struct entry {
    char key[MAX_KEY];
    int count;
    struct entry *next;
};

struct entry *table[TABLE_SIZE];

unsigned long djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = hash * 33 + c;
    return hash;
}

// TODO: 实现 hash_insert 函数
// 功能:
//   1. 计算桶下标
//   2. 遍历链表查找是否已存在:
//      - 已存在: count++
//      - 不存在: 创建新节点(count=1)，头插法
// void hash_insert(const char *key) { ... }

// TODO: 实现 collect_keys 函数
// 功能: 遍历整个哈希表，将所有 entry 的指针收集到数组中
//       返回收集到的数量
// int collect_keys(struct entry *results[], int max) { ... }

// TODO: 实现 compare 函数（用于 qsort）
// 功能: 按 key 字典序排序
// int compare(const void *a, const void *b) { ... }

int main(void)
{
    char line[MAX_LINE];

    // TODO: fgets 读一行
    // TODO: 去掉末尾换行
    // TODO: strtok 按空格分词，每个词 hash_insert
    // TODO: collect_keys 收集所有条目
    // TODO: qsort 按字典序排序
    // TODO: 打印 "key:count" 格式（空格分隔）
    // TODO: printf("\n")
    (void)line;

    return 0;
}
