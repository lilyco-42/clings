// 40a_dict_load.c — 字典加载: 从 stdin 读取 key=value 对存入哈希表
//
// 任务: 从 stdin 读取 "key value" 格式的行（空格分隔）
//       存入哈希表，最后输出加载的条目数
//
// 知识点: 哈希表综合应用、字符串解析、数据加载
//
// 示例: 输入:
//   apple 1
//   banana 2
//   cherry 3
// 输出: loaded: 3
//
// 预期输出（stdin="apple 1\nbanana 2\ncherry 3\n"）:
//   loaded: 3

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

// TODO: 实现 dict_put 函数
// 功能:
//   1. 计算桶下标
//   2. 如果 key 已存在，更新 value
//   3. 如果不存在，创建新节点头插法
//   返回: 1 表示新增，0 表示更新
// int dict_put(const char *key, const char *value) { ... }

int main(void)
{
    char line[MAX_LINE];
    int count = 0;

    // TODO: 循环 fgets 读取每一行
    // TODO: 用 sscanf(line, "%s %s", key, val) 解析
    // TODO: 调用 dict_put，如果返回 1 则 count++
    // TODO: 打印 printf("loaded: %d\n", count);
    (void)line;
    (void)count;

    return 0;
}
