/* 58_aho-corasick.c — Aho-Corasick 多模式匹配
 *
 * 任务：1. 实现 new_node() — 创建新节点
 *       2. 实现 insert() — 将模式串插入 Trie
 *       3. 实现 bfs_build_fail() — BFS 构建失败链接
 *       4. 实现 search() — 扫描文本输出匹配
 *       5. 实现 print_trie() — 打印 Trie 结构
 *       6. 补全 main() 主流程
 *
 * 知识点：Trie 树、失败链接 (Failure Link)、BFS 层序遍历、
 *         Aho-Corasick 自动机、多模式匹配
 *
 * 验证：固定模式集{"he","she","his","hers"}, 文本"ushers"
 *       make test 比对 expected_output.txt
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

static TrieNode nodes[MAX_NODES];
static int node_count = 0;

/* 创建新节点，返回节点索引 */
int new_node(void) {
    int idx = node_count++;
    for (int i = 0; i < ALPHABET; i++) nodes[idx].children[i] = -1;
    nodes[idx].fail = 0;
    nodes[idx].output_count = 0;
    return idx;
}

/* 将模式串 pattern 插入 Trie */
void insert(const char *pattern) {
    int cur = 0; /* 从根节点开始 */
    for (int i = 0; pattern[i]; i++) {
        int c = pattern[i] - 'a';
        if (nodes[cur].children[c] == -1) nodes[cur].children[c] = new_node();
        cur = nodes[cur].children[c];
    }
    /* 在终止节点记录输出 */
    int oc = nodes[cur].output_count;
    strcpy(nodes[cur].outputs[oc], pattern);
    nodes[cur].output_count++;
}

/* BFS 构建失败链接 */
void bfs_build_fail(void) {
    int queue[MAX_NODES];
    int head = 0, tail = 0;

    /* 根节点的所有直接子节点的 fail 指向根 */
    for (int c = 0; c < ALPHABET; c++) {
        int child = nodes[0].children[c];
        if (child != -1) {
            nodes[child].fail = 0;
            queue[tail++] = child;
        }
    }

    /* BFS 遍历 */
    while (head < tail) {
        int cur = queue[head++];

        for (int c = 0; c < ALPHABET; c++) {
            int child = nodes[cur].children[c];
            if (child == -1) continue;

            /* 计算 child 的 fail: 沿 cur 的 fail 链找有 c 子节点的祖先 */
            int f = nodes[cur].fail;
            while (f != 0 && nodes[f].children[c] == -1) f = nodes[f].fail;
            if (nodes[f].children[c] != -1 && nodes[f].children[c] != child)
                nodes[child].fail = nodes[f].children[c];
            else
                nodes[child].fail = 0;

            /* 合并 fail 链上的输出到当前节点 */
            int fail_node = nodes[child].fail;
            for (int i = 0; i < nodes[fail_node].output_count; i++) {
                int oc = nodes[child].output_count;
                strcpy(nodes[child].outputs[oc], nodes[fail_node].outputs[i]);
                nodes[child].output_count++;
            }

            queue[tail++] = child;
        }
    }
}

/* 扫描文本，输出每一步状态和匹配结果 */
void search(const char *text) {
    int cur = 0; /* 从根节点开始 */
    int text_len = strlen(text);

    printf("=== Scanning: \"%s\" ===\n", text);
    for (int i = 0; i < text_len; i++) {
        int c = text[i] - 'a';

        /* 沿 fail 链找到有 c 子节点的状态 */
        while (cur != 0 && nodes[cur].children[c] == -1) cur = nodes[cur].fail;

        if (nodes[cur].children[c] != -1) cur = nodes[cur].children[c];

        /* 输出当前状态信息 */
        printf("Step %d: char='%c' -> node %d", i, text[i], cur);
        if (nodes[cur].output_count > 0) {
            printf(" [match:");
            for (int j = 0; j < nodes[cur].output_count; j++) {
                int plen = strlen(nodes[cur].outputs[j]);
                printf(" %s@%d", nodes[cur].outputs[j], i - plen + 1);
            }
            printf(" ]");
        }
        printf("\n");
    }
    printf("\n");
}

/* 打印 Trie 结构：每节点的子节点和失败链接 */
void print_trie(void) {
    printf("=== Trie Structure ===\n");
    printf("Total nodes: %d\n\n", node_count);

    for (int i = 0; i < node_count; i++) {
        printf("Node %d: ", i);

        /* 打印子节点 */
        int has_child = 0;
        printf("children={");
        for (int c = 0; c < ALPHABET; c++) {
            if (nodes[i].children[c] != -1) {
                if (has_child) printf(", ");
                printf("'%c'->%d", c + 'a', nodes[i].children[c]);
                has_child = 1;
            }
        }
        printf("}");

        /* 打印失败链接 */
        printf(" fail=%d", nodes[i].fail);

        /* 打印输出 */
        if (nodes[i].output_count > 0) {
            printf(" output=[");
            for (int j = 0; j < nodes[i].output_count; j++) {
                if (j > 0) printf(", ");
                printf("\"%s\"", nodes[i].outputs[j]);
            }
            printf("]");
        }
        printf("\n");
    }
    printf("\n");
}

int main(void) {
    /* 初始化根节点 */
    node_count = 0;
    new_node(); /* 根节点索引 0 */

    /* 插入模式串 */
    const char *patterns[] = {"he", "she", "his", "hers"};
    int np = 4;
    for (int i = 0; i < np; i++) insert(patterns[i]);

    /* 构建失败链接 */
    bfs_build_fail();

    /* 打印 Trie 结构 */
    print_trie();

    /* 扫描文本 */
    const char *text = "ushers";
    search(text);

    /* 打印最终匹配结果汇总 */
    printf("=== Final Matches ===\n");
    int cur = 0;
    int text_len = strlen(text);
    for (int i = 0; i < text_len; i++) {
        int c = text[i] - 'a';
        while (cur != 0 && nodes[cur].children[c] == -1) cur = nodes[cur].fail;
        if (nodes[cur].children[c] != -1) cur = nodes[cur].children[c];
        for (int j = 0; j < nodes[cur].output_count; j++) {
            int plen = strlen(nodes[cur].outputs[j]);
            printf("Pattern \"%s\" found at position %d (ending at %d)\n", nodes[cur].outputs[j], i - plen + 1, i);
        }
    }

    return 0;
}
