// 40b_dict_lookup.c — 字典查找: 加载字典后查找指定 key
//
// 任务: 从 stdin 读取 "key value" 对，存入哈希表
//       然后查找命令行参数指定的 key，输出对应 value
//
// 知识点: 哈希表查找、命令行参数、数据存取分离
//
// 示例: stdin="apple 1\nbanana 2\ncherry 3\n", args=["banana"]
//       输出: banana: 2
//
// 预期输出:
//   banana: 2

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
// 功能: 插入或更新 key-value 对
// void dict_put(const char *key, const char *value) { ... }

// TODO: 实现 dict_get 函数
// 功能: 根据 key 查找对应的 value
//   找到返回 value 字符串指针，未找到返回 NULL
// const char *dict_get(const char *key) { ... }

int main(int argc, char *argv[])
{
    // TODO: 检查 argc >= 2，argv[1] 为要查找的 key
    // TODO: 循环 fgets 读取 stdin 中的 "key value" 行
    // TODO: dict_put 存入哈希表
    // TODO: 调用 dict_get(argv[1]) 查找
    // TODO: 打印 printf("%s: %s\n", argv[1], result);
    (void)argc;
    (void)argv;

    return 0;
}
