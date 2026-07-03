/* 58_aho-corasick.c — Aho-Corasick 多模式匹配
 *
 * 任务：1. 实现 new_node() — 创建新节点并初始化
 *       2. 实现 insert() — 将模式串插入 Trie 树
 *       3. 实现 bfs_build_fail() — BFS 构建失败链接
 *       4. 实现 print_trie() — 打印 Trie 结构
 *       5. 实现 search() — 扫描文本，输出每一步状态和匹配
 *       6. 补全 main() 主流程
 *
 * 背景：Aho-Corasick (AC 自动机) 由 Alfred Aho 和 Margaret Corasick
 *       在 1975 年提出。它通过 Trie+ 失败链接实现一次扫描匹配多个模式，
 *       时间复杂度 O(n+m+z), 其中 n=文本长度，m=总模式长度，z=匹配数。
 *
 * 知识点：Trie 树、失败链接 (Failure Link)、BFS 层序遍历、
 *         AC 自动机、多模式匹配、输出合并
 *
 * 验证：固定模式集 {"he","she","his","hers"}, 文本 "ushers"
 *       构建用 make; 判分/自测用 clings run 58 或 clings watch;
 *       查看期望输出用 clings tests 58
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALPHABET 26
#define MAX_NODES 30
#define MAX_PATTERNS 4
#define MAX_PAT_LEN 10

/* Trie 节点结构体：
 *   children[26] — 26 个小写字母子节点索引，-1 表示无
 *   fail         — 失败链接 (匹配失败时跳转到的节点)
 *   output_count — 该节点输出的模式数量
 *   outputs[][]  — 输出的模式名字符串数组
 */
typedef struct {
    int children[ALPHABET];
    int fail;
    int output_count;
    char outputs[MAX_PATTERNS][MAX_PAT_LEN];
} TrieNode;

static TrieNode nodes[MAX_NODES];
static int node_count = 0;

/* ─── TODO 1: 创建新节点 ─── */
#error TODO: Implement new_node() — create a new TrieNode
/*
 * static int new_node(void) {
 *     1. 保存当前 node_count 作为新节点索引
 *     2. node_count++
 *     3. 将 children[0..25] 全部初始化为 -1 (表示无子节点)
 *     4. fail 初始化为 0
 *     5. output_count 初始化为 0
 *     6. 返回新节点索引
 * }
 */

/* ─── TODO 2: 插入模式串到 Trie ─── */
#error TODO: Implement insert() — insert a pattern into the Trie
/*
 * static void insert(const char *pattern) {
 *     从根节点 (cur=0) 开始，遍历 pattern 每个字符：
 *     - 将字符转为索引 c = pattern[i] - 'a'
 *     - 若 children[c] == -1, 调用 new_node() 创建子节点
 *     - cur = children[c] 进入下一层
 *     遍历结束后，在 cur 节点记录输出：
 *     - 将 pattern 复制到 outputs[output_count]
 *     - output_count++
 * }
 */

/* ─── TODO 3: BFS 构建失败链接 ─── */
#error TODO: Implement bfs_build_fail() — build failure links via BFS
/*
 * static void bfs_build_fail(void) {
 *     准备队列 queue[MAX_NODES], head=0, tail=0
 *
 *     第一步：根节点 (0) 的直接子节点入队，fail 设为 0
 *     for c in 0..25:
 *         child = nodes[0].children[c]
 *         if child != -1:
 *             nodes[child].fail = 0
 *             queue[tail++] = child
 *
 *     第二步：BFS 主循环 while head < tail:
 *         cur = queue[head++]
 *         for c in 0..25:
 *             child = nodes[cur].children[c]
 *             if child == -1: continue
 *
 *             找 child 的 fail:
 *               f = nodes[cur].fail
 *               while f != 0 && nodes[f].children[c] == -1:
 *                   f = nodes[f].fail
 *               if nodes[f].children[c] != -1 && nodes[f].children[c] != child:
 *                   nodes[child].fail = nodes[f].children[c]
 *               else:
 *                   nodes[child].fail = 0
 *
 *             输出合并：将 fail 节点的所有 outputs 复制到 child 的 outputs
 *               (遍历 nodes[fail_node].outputs, 追加到 child 的 outputs)
 *
 *             queue[tail++] = child
 * }
 */

/* ─── TODO 4: 打印 Trie 结构 ─── */
#error TODO: Implement print_trie() — print all nodes with children, fail, output
/*
 * static void print_trie(void) {
 *     printf("=== Trie Structure ===\n");
 *     printf("Total nodes: %d\n\n", node_count);
 *     for i in 0..node_count-1:
 *         打印：Node i: children={...} fail=X output=[...]
 *         children 格式：'a'->1, 'b'->2 (跳过 -1 的子节点)
 *         output 格式："pattern1", "pattern2"
 * }
 */

/* ─── TODO 5: 扫描文本 ─── */
#error TODO: Implement search() — scan text step by step
/*
 * static void search(const char *text) {
 *     cur = 0  (从根开始)
 *     printf("=== Scanning: \"%s\" ===\n", text);
 *     for i in 0..strlen(text)-1:
 *         c = text[i] - 'a'
 *
 *         沿 fail 链找有 c 子节点的状态：
 *         while cur != 0 && nodes[cur].children[c] == -1:
 *             cur = nodes[cur].fail
 *         if nodes[cur].children[c] != -1:
 *             cur = nodes[cur].children[c]
 *
 *         打印：printf("Step %d: char='%c' -> node %d", i, text[i], cur)
 *         如果 nodes[cur].output_count > 0:
 *             打印 [match: 模式名@起始位置 ...]
 *             起始位置 = i - strlen(模式名) + 1
 * }
 */

/* ─── TODO 6: main() 主流程 ─── */
#error TODO: Complete main() — orchestrate the AC automaton
/*
 * int main(void) {
 *     node_count = 0; new_node();  // 初始化根节点
 *
 *     const char *patterns[] = {"he","she","his","hers"};
 *     循环 insert(patterns[i])  插入 4 个模式串
 *
 *     bfs_build_fail();          // 构建失败链接
 *     print_trie();              // 打印 Trie 结构
 *
 *     search("ushers");          // 扫描文本
 *
 *     打印最终匹配结果：
 *     printf("=== Final Matches ===\n");
 *     再次扫描 "ushers", 输出每个匹配的模式名、起始位置、结束位置
 *     printf("Pattern \"%s\" found at position %d (ending at %d)\n", ...)
 *
 *     return 0;
 * }
 */

int main(void) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    return 0;
}
