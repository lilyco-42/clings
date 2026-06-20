/*
 * Lesson 40c: 字典统计 — 参考答案
 */
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

void dict_put(const char *key, const char *value)
{
    unsigned long index = djb2(key) % TABLE_SIZE;
    struct entry *cur = table[index];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            strcpy(cur->value, value);
            return; /* 更新不增加计数 */
        }
        cur = cur->next;
    }
    struct entry *node = malloc(sizeof(struct entry));
    strcpy(node->key, key);
    strcpy(node->value, value);
    node->next = table[index];
    table[index] = node;
    total_entries++;
}

int get_max_depth(void)
{
    int max = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
        int depth = 0;
        struct entry *cur = table[i];
        while (cur != NULL) {
            depth++;
            cur = cur->next;
        }
        if (depth > max)
            max = depth;
    }
    return max;
}

int main(void)
{
    char line[MAX_LINE];

    while (fgets(line, MAX_LINE, stdin) != NULL) {
        char key[MAX_KEY], val[MAX_VAL];
        if (sscanf(line, "%s %s", key, val) == 2)
            dict_put(key, val);
    }

    printf("entries: %d\n", total_entries);
    printf("max_depth: %d\n", get_max_depth());

    return 0;
}
