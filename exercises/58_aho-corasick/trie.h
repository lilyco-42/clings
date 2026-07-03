/* trie.h — Trie / Aho-Corasick 自动机头文件
 *
 * 提供 TrieNode 结构体、常量以及 Aho-Corasick
 * 多模式匹配算法所需的函数声明。
 */
#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define ALPHABET 26
#define MAX_NODES 30
#define MAX_PATTERNS 4
#define MAX_PAT_LEN 10

/* Trie 节点：每个节点有 26 个子节点索引、一条失败链接，
 * 以及一个已匹配模式的输出列表。 */
typedef struct {
    int children[ALPHABET];                  /* 子节点索引, -1 表示无 */
    int fail;                                /* 失败链接 */
    int output_count;                        /* 输出模式数量 */
    char outputs[MAX_PATTERNS][MAX_PAT_LEN]; /* 输出模式名 */
} TrieNode;

/* 创建新节点, 返回其索引 */
int new_node(void);

/* 将模式串插入 Trie */
void insert(const char *pattern);

/* BFS 构建失败链接 */
void bfs_build_fail(void);

/* 扫描文本, 逐步打印状态转移与匹配结果 */
void search(const char *text);

/* 打印 Trie 结构 */
void print_trie(void);

#endif /* TRIE_H */
