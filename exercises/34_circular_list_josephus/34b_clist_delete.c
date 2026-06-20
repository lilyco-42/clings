// 34b_clist_delete.c — 循环链表删除
//
// 任务: 实现 clist_delete() 函数
//       从 stdin 读取 N 和 K，构建 1..N 的循环链表，
//       删除值为 K 的节点，打印剩余节点
//
// 知识点: 循环链表删除、唯一节点特殊处理、找前驱
//
// 用法: echo "5 3" | ./34b_clist_delete
// 预期输出:
//   1 2 4 5

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
//       唯一节点（item->next == item）: free 后返回 NULL
//       一般情况: 找前驱 prev（prev->next == item），
//                 prev->next = item->next，free item，返回 prev->next
// link clist_delete(link item) { ... }

int main(void)
{
    int n, k;
    scanf("%d %d", &n, &k);

    // 构建循环链表 1..n
    link cur = NULL;
    for (int i = 1; i <= n; i++) {
        link item = make_node(make_int_data(i));
        cur = clist_insert_after(cur, item);
    }
    /* cur 指向最后插入的节点(n)，cur->next 是第一个节点(1) */

    // TODO: 找到值为 k 的节点并删除
    //   从 cur->next（第一个节点）开始遍历，找到 data==k 的节点
    //   调用 clist_delete 删除该节点

    // TODO: 从第一个节点开始打印剩余节点
    //   使用 do-while 遍历，格式: "%d " 每个数字后跟空格，最后换行

    return 0;
}
