/*
 * Lesson 39d: 符号表（哈希表词频统计）— 参考答案
 */
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

void hash_insert(const char *key)
{
    unsigned long index = djb2(key) % TABLE_SIZE;
    struct entry *cur = table[index];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            cur->count++;
            return;
        }
        cur = cur->next;
    }
    struct entry *node = malloc(sizeof(struct entry));
    strcpy(node->key, key);
    node->count = 1;
    node->next = table[index];
    table[index] = node;
}

int collect_keys(struct entry *results[], int max)
{
    int n = 0;
    for (int i = 0; i < TABLE_SIZE && n < max; i++) {
        struct entry *cur = table[i];
        while (cur != NULL && n < max) {
            results[n++] = cur;
            cur = cur->next;
        }
    }
    return n;
}

int compare(const void *a, const void *b)
{
    struct entry *ea = *(struct entry **)a;
    struct entry *eb = *(struct entry **)b;
    return strcmp(ea->key, eb->key);
}

int main(void)
{
    char line[MAX_LINE];

    if (fgets(line, MAX_LINE, stdin) == NULL)
        return 0;

    /* 去掉末尾换行 */
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n')
        line[len - 1] = '\0';

    /* 按空格分词 */
    char *token = strtok(line, " ");
    while (token != NULL) {
        hash_insert(token);
        token = strtok(NULL, " ");
    }

    /* 收集并排序 */
    struct entry *results[1024];
    int n = collect_keys(results, 1024);
    qsort(results, (size_t)n, sizeof(struct entry *), compare);

    /* 打印 */
    for (int i = 0; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%s:%d", results[i]->key, results[i]->count);
    }
    printf("\n");

    return 0;
}
