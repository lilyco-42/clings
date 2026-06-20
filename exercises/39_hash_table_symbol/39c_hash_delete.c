// 39c_hash_delete.c — 哈希表删除: 从链式哈希表中删除元素
//
// 任务: 在 39b 基础上增加 delete 功能
//       插入 "a","b","c"，删除 "b"，查找 "a","b","c"
//
// 知识点: 链表删除操作 — 需要维护前驱指针
//
// 预期输出:
//   a: found
//   b: not found
//   c: found

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

void hash_insert(const char *key)
{
    unsigned long index = djb2(key) % TABLE_SIZE;
    /* 检查是否已存在 */
    struct entry *cur = table[index];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0)
            return;
        cur = cur->next;
    }
    /* 头插法 */
    struct entry *node = malloc(sizeof(struct entry));
    strcpy(node->key, key);
    node->next = table[index];
    table[index] = node;
}

int hash_lookup(const char *key)
{
    unsigned long index = djb2(key) % TABLE_SIZE;
    struct entry *cur = table[index];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0)
            return 1;
        cur = cur->next;
    }
    return 0;
}

// TODO: 实现 hash_delete 函数
// 功能:
//   1. 计算桶下标: index = djb2(key) % TABLE_SIZE
//   2. 遍历链表，找到 key 匹配的节点
//   3. 维护前驱指针 prev:
//      - 如果是头节点: table[index] = cur->next
//      - 否则: prev->next = cur->next
//   4. free 被删除的节点
// void hash_delete(const char *key) { ... }

int main(void)
{
    hash_insert("a");
    hash_insert("b");
    hash_insert("c");

    // TODO: hash_delete("b");

    // TODO: 查找并打印 "a", "b", "c"
    // printf("%s: %s\n", word, hash_lookup(word) ? "found" : "not found");

    return 0;
}
