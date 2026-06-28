/* 52_bplus-tree-index — B+ 树索引
 *
 * 实现 B+ 树（ORDER=3, MAX_KEY=2），完成插入与查找。
 *
 * 数据结构（已提供）:
 *   BPTreeNode { is_leaf, num_keys, keys[3], values[3] 或 children[4], next }
 *   - 内部节点：keys[] 存路由键，children[] 存子节点指针
 *   - 叶子节点：keys[] 存键，values[] 存值，next 形成链表
 *   - 数组多 1 位用于分裂前的临时溢出
 *
 * 你需要完成：
 *   1. create_node()     — 分配并初始化节点（叶子/内部）
 *   2. insert_into_leaf()— 在有序叶子中插入 key/value
 *   3. split_leaf_child()— 分裂满叶子（3 key → 2+1）
 *   4. insert_rec()      — 递归插入，处理溢出与分裂
 *   5. insert()          — 入口：空树建叶 / 递归插入 / 根溢出建新根
 *   6. search()          — 从根走到叶子，线性查找 key
 *   7. print_tree()      — 递归缩进打印树结构
 *
 * 编译：make
 * 测试：make test (与 expected_output.txt 比对)
 * 提示：clings hint
 */
#include <stdio.h>
#include <stdlib.h>

#define ORDER 3
#define MAX_KEY (ORDER - 1) /* = 2, 节点最多 2 个 key */

/* ---------- B+ 树节点 ---------- */
typedef struct BPTreeNode {
    int is_leaf;
    int num_keys;
    int keys[MAX_KEY + 1]; /* +1 容纳分裂前的溢出 */
    union {
        struct BPTreeNode *children[ORDER + 1];
        int values[MAX_KEY + 1];
    };
    struct BPTreeNode *next; /* 叶子链表指针 */
} BPTreeNode;

/* 前向声明 */
static BPTreeNode *create_node(int is_leaf);
static void insert_into_leaf(BPTreeNode *leaf, int key, int value);
static void insert_into_inner(BPTreeNode *inner, int idx, int key, BPTreeNode *left, BPTreeNode *right);
static void split_leaf_child(BPTreeNode *parent, int idx, BPTreeNode *child);
static BPTreeNode *insert_rec(BPTreeNode *node, int key, int value);
static BPTreeNode *insert(BPTreeNode *root, int key, int value);
static int search(BPTreeNode *root, int key);
static void print_tree(BPTreeNode *root, int depth);
static void free_tree(BPTreeNode *root);

/* ================================================================
 * TODO 1: create_node(is_leaf)
 *
 * 用 malloc 分配 BPTreeNode，初始化所有字段：
 *   - is_leaf = 参数值
 *   - num_keys = 0
 *   - next = NULL
 *   - 若 is_leaf: values[0..MAX_KEY] 置 0
 *   - 若 !is_leaf: children[0..ORDER] 置 NULL
 *
 * 提示：用 for 循环清零数组，避免越界（数组长度 = MAX_KEY+1 或 ORDER+1）
 * ================================================================ */
static BPTreeNode *create_node(int is_leaf) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ================================================================
 * TODO 2: insert_into_leaf(leaf, key, value)
 *
 * 将 key/value 按升序插入叶子节点。
 * 1. 找到插入位置 pos（第一个 keys[pos] >= key 的位置）
 * 2. 将 pos 及之后的元素后移一位（从后往前遍历）
 * 3. 在 pos 处写入新 key 和 value
 * 4. num_keys++
 *
 * 注意：同时移动 keys[] 和 values[]；数组容量足够（MAX_KEY+1）
 * ================================================================ */
static void insert_into_leaf(BPTreeNode *leaf, int key, int value) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ---------- 在内部节点 idx 位置插入 key 和左右孩子（已提供）---------- */
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

/* ================================================================
 * TODO 3: split_leaf_child(parent, idx, child)
 *
 * child 是一个溢出的叶子节点（num_keys = MAX_KEY + 1 = 3）。
 * 分裂策略：
 *   1. 创建 new_leaf（create_node(1)）
 *   2. 将 child 的后半部分 key/value 复制到 new_leaf
 *      - 3 个 key 时：左半保留 2 个，右半移走 1 个
 *      - child->keys[2] 和 child->values[2] → new_leaf
 *   3. 更新 new_leaf->num_keys 和 child->num_keys
 *   4. 维护叶子链表：new_leaf->next = child->next; child->next = new_leaf;
 *   5. 调用 insert_into_inner(parent, idx, up_key, child, new_leaf)
 *      其中 up_key = new_leaf->keys[0]（右半第一个 key）
 *
 * 提示：右半第一个 key 是 new_leaf->keys[0]，也是上移到父节点的 key。
 * ================================================================ */
static void split_leaf_child(BPTreeNode *parent, int idx, BPTreeNode *child) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ================================================================
 * TODO 4: insert_rec(node, key, value)
 *
 * 递归插入。返回 NULL 表示无需分裂，返回节点指针表示该节点溢出。
 *
 * 若 node 是叶子：
 *   - 调用 insert_into_leaf(node, key, value)
 *   - 若 num_keys > MAX_KEY: return node（溢出）
 *   - 否则 return NULL
 *
 * 若 node 是内部节点：
 *   - 找到 key 应走的子节点位置 pos
 *     （while (pos < num_keys && keys[pos] <= key) pos++）
 *   - 递归调用 insert_rec(node->children[pos], key, value)
 *   - 若返回值非 NULL（子节点溢出）:
 *       调用 split_leaf_child(node, pos, 返回值) 分裂子节点
 *       若分裂后 node->num_keys > MAX_KEY: return node（自身溢出）
 *   - return NULL
 *
 * 提示：内部节点的路由规则：keys[pos] <= key 时 pos++，
 *       即"key 大于等于分隔键则走右边"。
 * ================================================================ */
static BPTreeNode *insert_rec(BPTreeNode *node, int key, int value) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ================================================================
 * TODO 5: insert(root, key, value)
 *
 * B+ 树插入入口：
 *   1. 若 root == NULL: 创建叶子节点，放入 key/value，返回
 *   2. 否则：调用 insert_rec(root, key, value)
 *   3. 若 insert_rec 返回非 NULL（根溢出）:
 *      - 创建新的内部节点 new_root
 *      - new_root->children[0] = root
 *      - 调用 split_leaf_child(new_root, 0, 溢出的旧根)
 *      - 返回 new_root
 *   4. 否则返回 root
 *
 * 提示：根溢出是 B+ 树高度增长的唯一途径。
 * ================================================================ */
static BPTreeNode *insert(BPTreeNode *root, int key, int value) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ================================================================
 * TODO 6: search(root, key)
 *
 * 查找 key 对应的 value，未找到返回 -1。
 *
 * 1. 若 root == NULL: return -1
 * 2. 从根走到叶子：
 *    while (!cur->is_leaf) {
 *        找 pos: while (pos < num_keys && keys[pos] <= key) pos++
 *        cur = cur->children[pos]
 *    }
 * 3. 在叶子中线性查找 key，找到返回 values[i]，否则返回 -1
 *
 * 提示：内部节点路由规则与 insert_rec 一致。
 * ================================================================ */
static int search(BPTreeNode *root, int key) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ================================================================
 * TODO 7: print_tree(root, depth)
 *
 * 递归缩进打印 B+ 树结构。
 *
 * - 若 root == NULL: 打印 "(empty tree)\n"
 * - 缩进：depth 个 "  "（两个空格）
 * - 叶子节点格式：
 *   "[leaf] keys: k0 k1 ... | values: v0 v1 ...\n"
 * - 内部节点格式：
 *   "[inner] keys: k0 k1 ...\n"
 *   然后递归打印每个 children[i]（depth+1）
 * - children 数量 = num_keys + 1
 *
 * 提示：叶子用 for 循环遍历 num_keys 个 key/value 打印。
 * ================================================================ */
static void print_tree(BPTreeNode *root, int depth) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ---------- 释放整棵树（已提供）---------- */
static void free_tree(BPTreeNode *root) {
    if (root == NULL) return;
    if (!root->is_leaf) {
        for (int i = 0; i <= root->num_keys; i++) free_tree(root->children[i]);
    }
    free(root);
}

/* ========== 主流程（已提供）========== */
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
