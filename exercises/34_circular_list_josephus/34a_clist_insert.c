// 34a_clist_insert.c — 循环链表插入
//
// 任务: 实现 clist_insert_after() 函数
//       从 stdin 读取 N，构建包含 1..N 的循环链表，输出节点数
//
// 知识点: 循环链表的自环初始化、插入到 cur 之后
//
// 用法: echo "5" | ./34a_clist_insert
// 预期输出:
//   5

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

// TODO: 实现 clist_insert_after 函数
// 功能: 将 item 插入到 cur 之后
//       特殊情况: cur==NULL 时，item->next = item（自环），返回 item
//       一般情况: item->next = cur->next; cur->next = item; 返回 item
// link clist_insert_after(link cur, link item) { ... }

// TODO: 实现 clist_length 函数
// 功能: 使用 do-while 遍历循环链表，计算节点个数
//       如果 cur == NULL，返回 0
// int clist_length(link cur) { ... }

int main(void)
{
    int n;
    scanf("%d", &n);

    link cur = NULL;

    // TODO: 循环 1..n，创建节点并用 clist_insert_after 插入
    //       每次插入后 cur 指向新插入的节点（尾部）
    // TODO: 输出链表长度
    //       printf("%d\n", clist_length(cur));

    return 0;
}
