// 39b_hash_insert.c — 链式哈希表: 插入与查找
//
// 任务: 实现基于链表的哈希表，支持 insert 和 lookup
//       插入 "apple","banana","cherry"
//       查找 "banana" 和 "grape"
//
// 知识点: 哈希冲突解决 — 链地址法（每个桶是一个链表）
//
// 预期输出:
//   banana: found
//   grape: not found

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 16
#define MAX_KEY 64

struct entry {
    char key[MAX_KEY];
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
//   1. 计算桶下标: index = djb2(key) % TABLE_SIZE
//   2. 检查该桶链表中是否已存在相同 key（用 strcmp）
//   3. 如果不存在，创建新 entry 插入链表头部
// void hash_insert(const char *key) { ... }

// TODO: 实现 hash_lookup 函数
// 功能:
//   1. 计算桶下标: index = djb2(key) % TABLE_SIZE
//   2. 遍历该桶链表，找到 key 相同的节点返回 1
//   3. 未找到返回 0
// int hash_lookup(const char *key) { ... }

int main(void)
{
    // TODO: hash_insert("apple");
    // TODO: hash_insert("banana");
    // TODO: hash_insert("cherry");

    // TODO: 查找 "banana" 和 "grape"，打印结果
    // printf("%s: %s\n", word, hash_lookup(word) ? "found" : "not found");

    return 0;
}
