/* bptree.h — B+ Tree header
 *
 * Provides BPTreeNode structure, ORDER/MAX_KEY constants, and
 * function declarations for B+ tree operations.
 */

#ifndef BPTREE_H
#define BPTREE_H

#define ORDER 3
#define MAX_KEY (ORDER - 1) /* = 2, max keys per node */

/* B+ Tree node:
 *   - is_leaf: 1 for leaf, 0 for internal node
 *   - num_keys: current number of keys stored
 *   - keys[]: routing keys (internal) or data keys (leaf)
 *   - children[]: child pointers (internal nodes only)
 *   - values[]: data values (leaf nodes only)
 *   - next: linked-list pointer for leaf nodes (range scans)
 *
 * Arrays are sized +1 to hold overflow during split. */
typedef struct BPTreeNode {
    int is_leaf;
    int num_keys;
    int keys[MAX_KEY + 1];
    union {
        struct BPTreeNode *children[ORDER + 1];
        int values[MAX_KEY + 1];
    };
    struct BPTreeNode *next;
} BPTreeNode;

/* Create a new node (leaf or internal) */
BPTreeNode *create_node(int is_leaf);

/* Insert key/value pair into the B+ tree rooted at *root */
BPTreeNode *insert(BPTreeNode *root, int key, int value);

/* Search for key; return associated value, or -1 if not found */
int search(BPTreeNode *root, int key);

/* Print tree structure with indentation */
void print_tree(BPTreeNode *root, int depth);

#endif /* BPTREE_H */
