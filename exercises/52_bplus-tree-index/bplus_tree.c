/* 52_bplus-tree-index — B+ 树索引
 *
 * 实现 B+ 树（ORDER=3, MAX_KEY=2），完成插入（含叶子分裂与内部节点分裂）与查找。
 *
 * 数据结构（已在 bptree.h 中提供，勿在本文件重复定义）:
 *   BPTreeNode { is_leaf, num_keys, keys[3], values[3] 或 children[4], next }
 *   - 内部节点：keys[] 存路由键，children[] 存子节点指针
 *   - 叶子节点：keys[] 存键，values[] 存值，next 形成链表
 *   - 数组多 1 位用于分裂前的临时溢出
 *
 * 你需要完成（8 个函数）:
 *   1. create_node()      — 分配并初始化节点（叶子/内部）
 *   2. insert_into_leaf() — 在有序叶子中插入 key/value
 *   3. split_leaf_child() — 分裂满叶子（3 key → 2+1，右半首键上移）
 *   4. split_inner_child()— 分裂满内部节点（3 key → 1+1，中间键上推）
 *   5. insert_rec()       — 递归插入，处理溢出与分裂
 *   6. insert()           — 入口：空树建叶 / 递归插入 / 根溢出建新根
 *   7. search()           — 从根走到叶子，线性查找 key
 *   8. print_tree()       — 递归缩进打印树结构
 *
 * 编译：make      （SRC = bplus_tree.c；bptree.h 与源码同目录，引号包含自动找到）
 * 判分/自测：clings run / clings watch（clings 捕获程序 stdout 与内置用例逐行比对；
 *            期望输出可用 clings tests 52 查看）
 * 提示：clings hint
 */
#include <stdio.h>
#include <stdlib.h>

#include "bptree.h" /* 提供 BPTreeNode 类型、ORDER/MAX_KEY 常量与对外函数原型(与参考答案同结构) */

/* 前向声明（静态辅助函数；create_node/insert/search/print_tree 的原型在 bptree.h 中）*/
static void insert_into_leaf(BPTreeNode *leaf, int key, int value);
static void insert_into_inner(BPTreeNode *inner, int idx, int key, BPTreeNode *left, BPTreeNode *right);
static void split_leaf_child(BPTreeNode *parent, int idx, BPTreeNode *child);
static void split_inner_child(BPTreeNode *parent, int idx, BPTreeNode *child);
static void split_child(BPTreeNode *parent, int idx, BPTreeNode *child);
static BPTreeNode *insert_rec(BPTreeNode *node, int key, int value);
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
BPTreeNode *create_node(int is_leaf) {
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
 * TODO 4: split_inner_child(parent, idx, child)
 *
 * child 是一个溢出的内部节点（num_keys = MAX_KEY+1 = 3，有 4 个 children）。
 * 与叶子分裂的关键区别：内部分裂把【中间键上推】给父节点（不是复制）。
 *
 *   child->keys:     [k0, k1, k2]
 *   child->children: [c0, c1, c2, c3]
 *
 * 分裂策略：
 *   1. 创建 new_inner = create_node(0)
 *   2. up_key = child->keys[1]                 // 中间键，上推给父节点
 *   3. 右半 → new_inner:
 *        new_inner->keys[0]     = child->keys[2]
 *        new_inner->children[0] = child->children[2]
 *        new_inner->children[1] = child->children[3]
 *        new_inner->num_keys    = 1
 *   4. 左半 → child 只保留 keys[0] 和 children[0], children[1]:
 *        child->num_keys = 1
 *   5. 上推: insert_into_inner(parent, idx, up_key, child, new_inner)
 *
 * 对比：叶子分裂"复制"右半首键上移（数据仍留在叶子）；
 *       内部分裂"移动"中间键上推（k1 离开 child，不再出现在子层）。
 * ================================================================ */
static void split_inner_child(BPTreeNode *parent, int idx, BPTreeNode *child) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ---------- 分裂子节点（按类型分发，已提供）---------- */
static void split_child(BPTreeNode *parent, int idx, BPTreeNode *child) {
    if (child->is_leaf)
        split_leaf_child(parent, idx, child);
    else
        split_inner_child(parent, idx, child);
}

/* ================================================================
 * TODO 5: insert_rec(node, key, value)
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
 *       调用 split_child(node, pos, 返回值) 分裂子节点
 *         （split_child 会根据子节点是叶子还是内部，分别走叶子/内部分裂）
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
 * TODO 6: insert(root, key, value)
 *
 * B+ 树插入入口：
 *   1. 若 root == NULL: 创建叶子节点，放入 key/value，返回
 *   2. 否则：调用 insert_rec(root, key, value)
 *   3. 若 insert_rec 返回非 NULL（根溢出）:
 *      - 创建新的内部节点 new_root
 *      - new_root->children[0] = root
 *      - 调用 split_child(new_root, 0, 溢出的旧根)
 *        （旧根可能是叶子，也可能是内部节点——split_child 自动分发）
 *      - 返回 new_root
 *   4. 否则返回 root
 *
 * 提示：根溢出是 B+ 树高度增长的唯一途径。
 * ================================================================ */
BPTreeNode *insert(BPTreeNode *root, int key, int value) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ================================================================
 * TODO 7: search(root, key)
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
int search(BPTreeNode *root, int key) {
#error TODO: Finish this exercise. Run "clings hint" for help.
}

/* ================================================================
 * TODO 8: print_tree(root, depth)
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
void print_tree(BPTreeNode *root, int depth) {
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

    /* ── 插入阶段 ──
     * 序列 10,30,20,40,50,60,70：
     *   insert(20) 需在叶子中间插入(演示 insert_into_leaf 的移位)；
     *   insert(20/50/70) 触发叶子分裂；
     *   insert(70) 使根(内部节点)溢出 → 触发内部节点分裂，树高 2→3。 */
    int inserts[][2] = {
        {10, 100}, {30, 300}, {20, 200}, {40, 400}, {50, 500}, {60, 600}, {70, 700},
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

    val = search(root, 40);
    printf("search(40) = %d\n", val);

    val = search(root, 45);
    printf("search(45) = %d\n", val);

    free_tree(root);
    return 0;
}
