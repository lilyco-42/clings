/* 52_bplus-tree-index — solution
 *
 * B+ 树索引实现：多路平衡树（ORDER=3, max 2 keys），叶子节点链表连接。
 *
 * 数据结构：
 *   - 内部节点：最多 ORDER-1=2 个 key，最多 ORDER=3 个子节点
 *   - 叶子节点：最多 ORDER-1=2 个 key，最多 ORDER-1=2 个 value
 *              next 指针形成链表
 *
 * B+ 树核心性质：
 *   1. 内部节点只存 key 用于路由，叶子节点存 key+value
 *   2. 叶子节点通过 next 指针串联成有序链表
 *   3. 所有叶子节点在同一层（完美平衡）
 *
 * 知识点：多路平衡树、叶子节点链表、范围查询、磁盘友好数据结构
 */
#include <stdio.h>
#include <stdlib.h>

#include "bptree.h"

#define MIN_KEY (ORDER / 2) /* 节点最小 key 数 = 1 */

/* 前向声明 */
static void insert_into_leaf(BPTreeNode *leaf, int key, int value);
static void split_leaf_child(BPTreeNode *parent, int idx, BPTreeNode *child);
static void split_inner_child(BPTreeNode *parent, int idx, BPTreeNode *child);
static void split_child(BPTreeNode *parent, int idx, BPTreeNode *child);
static BPTreeNode *insert_rec(BPTreeNode *node, int key, int value);
static void free_tree(BPTreeNode *root);

/* ---------- 创建新节点 ---------- */
BPTreeNode *create_node(int is_leaf) {
    BPTreeNode *node = malloc(sizeof(BPTreeNode));
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    node->next = NULL;
    if (is_leaf) {
        for (int i = 0; i <= MAX_KEY; i++) node->values[i] = 0;
    } else {
        for (int i = 0; i <= ORDER; i++) node->children[i] = NULL;
    }
    return node;
}

/* ---------- 在叶子节点中插入 key/value（保持有序）---------- */
static void insert_into_leaf(BPTreeNode *leaf, int key, int value) {
    int pos = 0;
    while (pos < leaf->num_keys && leaf->keys[pos] < key) pos++;
    for (int i = leaf->num_keys; i > pos; i--) {
        leaf->keys[i] = leaf->keys[i - 1];
        leaf->values[i] = leaf->values[i - 1];
    }
    leaf->keys[pos] = key;
    leaf->values[pos] = value;
    leaf->num_keys++;
}

/* ---------- 在内部节点 idx 位置插入 key 和左右孩子 ---------- */
static void insert_into_inner(BPTreeNode *inner, int idx, int key, BPTreeNode *left, BPTreeNode *right) {
    for (int i = inner->num_keys; i > idx; i--) {
        inner->keys[i] = inner->keys[i - 1];
        inner->children[i + 1] = inner->children[i];
    }
    inner->keys[idx] = key;
    inner->children[idx] = left;
    inner->children[idx + 1] = right;
    inner->num_keys++;
}

/* ---------- 分裂满的叶子节点（child 有 MAX_KEY+1 = 3 个 key）---------- */
static void split_leaf_child(BPTreeNode *parent, int idx, BPTreeNode *child) {
    BPTreeNode *new_leaf = create_node(1);

    /* child 有 3 个 key: 左半 2 个，右半 1 个 */
    new_leaf->keys[0] = child->keys[2];
    new_leaf->values[0] = child->values[2];
    new_leaf->num_keys = 1;
    child->num_keys = 2;

    /* 更新叶子链表 */
    new_leaf->next = child->next;
    child->next = new_leaf;

    /* 上移 key = 右半第一个 key */
    insert_into_inner(parent, idx, new_leaf->keys[0], child, new_leaf);
}

/* ---------- 分裂满的内部节点（child 有 MAX_KEY+1 = 3 个 key, ORDER+1 = 4 个孩子）---------- */
static void split_inner_child(BPTreeNode *parent, int idx, BPTreeNode *child) {
    BPTreeNode *new_inner = create_node(0);

    /* child 有 3 个 key[0,1,2] 和 4 个 children[0,1,2,3]
     * 上移 keys[1], 左半：keys[0]+children[0,1], 右半：keys[2]+children[2,3] */
    int up_key = child->keys[1];

    new_inner->keys[0] = child->keys[2];
    new_inner->children[0] = child->children[2];
    new_inner->children[1] = child->children[3];
    new_inner->num_keys = 1;

    child->num_keys = 1; /* 只保留 keys[0], children[0,1] */

    insert_into_inner(parent, idx, up_key, child, new_inner);
}

/* ---------- 分裂子节点（根据类型分发）---------- */
static void split_child(BPTreeNode *parent, int idx, BPTreeNode *child) {
    if (child->is_leaf)
        split_leaf_child(parent, idx, child);
    else
        split_inner_child(parent, idx, child);
}

/* ---------- 递归插入 ---------- */
static BPTreeNode *insert_rec(BPTreeNode *node, int key, int value) {
    if (node->is_leaf) {
        insert_into_leaf(node, key, value);
        if (node->num_keys > MAX_KEY) return node; /* 溢出 */
        return NULL;
    }

    /* 内部节点：找子节点 */
    int pos = 0;
    while (pos < node->num_keys && node->keys[pos] <= key) pos++;
    BPTreeNode *overflow = insert_rec(node->children[pos], key, value);

    if (overflow != NULL) {
        split_child(node, pos, overflow);
        if (node->num_keys > MAX_KEY) return node;
    }
    return NULL;
}

/* ---------- 插入 key/value 到 B+ 树 ---------- */
BPTreeNode *insert(BPTreeNode *root, int key, int value) {
    if (root == NULL) {
        root = create_node(1);
        root->keys[0] = key;
        root->values[0] = value;
        root->num_keys = 1;
        return root;
    }

    BPTreeNode *overflow = insert_rec(root, key, value);

    if (overflow != NULL) {
        /* 根溢出 → 创建新根 */
        BPTreeNode *new_root = create_node(0);
        new_root->children[0] = root;
        split_child(new_root, 0, overflow);
        return new_root;
    }

    return root;
}

/* ---------- 查找 key 对应的 value，未找到返回 -1 ---------- */
int search(BPTreeNode *root, int key) {
    if (root == NULL) return -1;

    BPTreeNode *cur = root;
    while (!cur->is_leaf) {
        int pos = 0;
        while (pos < cur->num_keys && cur->keys[pos] <= key) pos++;
        cur = cur->children[pos];
    }

    /* 在叶子中线性查找 */
    for (int i = 0; i < cur->num_keys; i++) {
        if (cur->keys[i] == key) return cur->values[i];
    }
    return -1;
}

/* ---------- 递归打印树结构 ---------- */
void print_tree(BPTreeNode *root, int depth) {
    if (root == NULL) {
        printf("(empty tree)\n");
        return;
    }

    for (int i = 0; i < depth; i++) printf("  ");

    if (root->is_leaf) {
        printf("[leaf] keys:");
        for (int i = 0; i < root->num_keys; i++) printf(" %d", root->keys[i]);
        printf(" | values:");
        for (int i = 0; i < root->num_keys; i++) printf(" %d", root->values[i]);
        printf("\n");
    } else {
        printf("[inner] keys:");
        for (int i = 0; i < root->num_keys; i++) printf(" %d", root->keys[i]);
        printf("\n");
        for (int i = 0; i <= root->num_keys; i++) print_tree(root->children[i], depth + 1);
    }
}

/* ---------- 释放整棵树 ---------- */
static void free_tree(BPTreeNode *root) {
    if (root == NULL) return;
    if (!root->is_leaf) {
        for (int i = 0; i <= root->num_keys; i++) free_tree(root->children[i]);
    }
    free(root);
}

/* ========== 主流程 ========== */
int main(void) {
    printf("=== B+ Tree Index (ORDER=3, MAX_KEY=2) ===\n\n");

    BPTreeNode *root = NULL;

    /* ── 插入阶段 ── */
    int inserts[][2] = {
        {10, 100}, {20, 200}, {5, 50}, {15, 150}, {25, 250},
    };
    int n_inserts = sizeof(inserts) / sizeof(inserts[0]);

    for (int i = 0; i < n_inserts; i++) {
        int key = inserts[i][0];
        int value = inserts[i][1];
        root = insert(root, key, value);
        printf("After insert(%d, v%d):\n", key, i);
        print_tree(root, 0);
        printf("\n");
    }

    /* ── 查找阶段 ── */
    printf("=== Search Results ===\n");
    int val;

    val = search(root, 15);
    printf("search(15) = %d\n", val);

    val = search(root, 30);
    printf("search(30) = %d\n", val);

    free_tree(root);
    return 0;
}
