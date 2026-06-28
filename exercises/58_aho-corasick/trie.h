/* trie.h — Trie / Aho-Corasick automaton header
 *
 * Provides TrieNode structure, constants, and function
 * declarations for the Aho-Corasick multi-pattern matching algorithm.
 */
#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define ALPHABET 26
#define MAX_NODES 30
#define MAX_PATTERNS 4
#define MAX_PAT_LEN 10

/* Trie node: each node has 26 child pointers, a failure link,
 * and an output list of matched patterns. */
typedef struct {
    int children[ALPHABET];                  /* child indices, -1 means none */
    int fail;                                /* failure link */
    int output_count;                        /* number of output patterns */
    char outputs[MAX_PATTERNS][MAX_PAT_LEN]; /* output pattern names */
} TrieNode;

/* Create a new node, return its index */
int new_node(void);

/* Insert a pattern into the trie */
void insert(const char *pattern);

/* BFS build failure links */
void bfs_build_fail(void);

/* Scan text and print step-by-step state transitions and matches */
void search(const char *text);

/* Print the trie structure */
void print_trie(void);

#endif /* TRIE_H */
