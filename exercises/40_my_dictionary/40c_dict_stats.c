// 40c_dict_stats.c — 字典统计: 输出哈希表分布信息
//
// 任务: 从 stdin 读取 "key value" 对，存入哈希表
//       输出统计信息: 总条目数和最大桶深度
//       （如果同一个 key 出现多次，只算一条）
//
// 知识点: 哈希表性能分析、冲突检测、桶深度统计
//
// 示例: 输入:
//   apple 1
//   banana 2
//   cherry 3
//   apple 4       ← 重复 key，更新 value，不增加条目数
// 输出:
//   entries: 3
//   max_depth: 1
//
// 预期输出（stdin="apple 1\nbanana 2\ncherry 3\napple 4\n"）:
//   entries: 3
//   max_depth: 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 16
#define MAX_KEY 64
#define MAX_VAL 64
#define MAX_LINE 256

struct entry {
    char key[MAX_KEY];
    char value[MAX_VAL];
    struct entry *next;
};

struct entry *table[TABLE_SIZE];
int total_entries = 0;

unsigned long djb2(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = hash * 33 + c;
    return hash;
}

// TODO: 实现 dict_put 函数
// 功能: 插入或更新 key-value 对
//   - 如果 key 已存在: 更新 value（不增加 total_entries）
//   - 如果 key 不存在: 新建节点，total_entries++
// void dict_put(const char *key, const char *value) { ... }

// TODO: 实现 get_max_depth 函数
// 功能: 遍历所有桶，找出最长链表长度
// int get_max_depth(void) { ... }

int main(void)
{
    char line[MAX_LINE];

    // TODO: 循环 fgets 读取每一行
    // TODO: sscanf 解析 key 和 value
    // TODO: dict_put(key, value)

    // TODO: printf("entries: %d\n", total_entries);
    // TODO: printf("max_depth: %d\n", get_max_depth());
    (void)line;

    return 0;
}
