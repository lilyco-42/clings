/*
 * Lesson 39c: 哈希表删除 — 参考答案
 */
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
    struct entry *cur = table[index];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0)
            return;
        cur = cur->next;
    }
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

void hash_delete(const char *key)
{
    unsigned long index = djb2(key) % TABLE_SIZE;
    struct entry *cur = table[index];
    struct entry *prev = NULL;

    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            if (prev == NULL)
                table[index] = cur->next;
            else
                prev->next = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

int main(void)
{
    hash_insert("a");
    hash_insert("b");
    hash_insert("c");

    hash_delete("b");

    const char *queries[] = {"a", "b", "c"};
    for (int i = 0; i < 3; i++)
        printf("%s: %s\n", queries[i],
               hash_lookup(queries[i]) ? "found" : "not found");

    return 0;
}
