// 34b_josephus.c — 约瑟夫环问题
//
// 任务: 使用循环链表解决约瑟夫环问题
//       从 stdin 读取 N 和 K，输出淘汰顺序
//
// 知识点: 循环链表删除、计数器逻辑
//
// 用法: echo "10 3" | ./34b_josephus
// 预期输出:
//   3 6 9 2 7 1 8 5 10 4

#include <stdio.h>
#include <stdlib.h>

struct node {
    void *data;
    struct node *next;
};

typedef struct node *link;

link make_node(void *data)
{
    link p = malloc(sizeof(*p));
    p->data = data;
    p->next = NULL;
    return p;
}

int *make_int_data(int val)
{
    int *p = malloc(sizeof(int));
    *p = val;
    return p;
}

link clist_insert_after(link cur, link item)
{
    if (cur == NULL) {
        item->next = item;
        return item;
    }
    item->next = cur->next;
    cur->next = item;
    return item;
}

// TODO: 实现 clist_delete 函数
// 功能: 从循环链表中删除 item 节点
//       找到 item 的前驱节点 prev（prev->next == item）
//       然后 prev->next = item->next
//       如果只剩一个节点（item->next == item），返回 NULL
//       否则返回 prev（作为新的当前位置）
// link clist_delete(link cur, link item) { ... }

// TODO: 实现约瑟夫环求解
// 算法:
//   1. 构建 1..N 的循环链表
//   2. cur 指向最后一个节点（N），这样计数从 1 开始
//   3. 重复直到链表为空:
//      a. 从 cur 开始走 K 步，到达要淘汰的节点
//      b. 打印该节点的数据
//      c. 删除该节点，cur 更新为删除后返回的位置

int main(void)
{
    int n, k;
    scanf("%d %d", &n, &k);

    // TODO: 构建循环链表 1..n
    // TODO: 执行约瑟夫环淘汰过程，输出淘汰顺序
    // 格式: 每个数字后跟空格，最后打印换行

    return 0;
}
