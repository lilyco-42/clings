/*
 * Lesson 40a: 字典加载 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 256
#define MAX_KEY 64
#define MAX_VAL 64
#define MAX_LINE 256

struct entry {
    char key[MAX_KEY];
    char value[MAX_VAL];
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

int dict_put(const char *key, const char *value)
{
    unsigned long index = djb2(key) % TABLE_SIZE;
    struct entry *cur = table[index];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            strcpy(cur->value, value);
            return 0; /* 更新，非新增 */
        }
        cur = cur->next;
    }
    struct entry *node = malloc(sizeof(struct entry));
    strcpy(node->key, key);
    strcpy(node->value, value);
    node->next = table[index];
    table[index] = node;
    return 1; /* 新增 */
}

int main(void)
{
    char line[MAX_LINE];
    int count = 0;

    while (fgets(line, MAX_LINE, stdin) != NULL) {
        char key[MAX_KEY], val[MAX_VAL];
        if (sscanf(line, "%s %s", key, val) == 2) {
            if (dict_put(key, val))
                count++;
        }
    }

    printf("loaded: %d\n", count);

    return 0;
}
