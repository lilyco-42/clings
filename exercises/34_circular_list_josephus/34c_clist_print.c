// 34c_clist_print.c — 循环链表 do-while 遍历打印
//
// 任务: 实现 clist_print() 函数
//       使用 do-while 循环遍历循环链表并打印所有节点
//
// 知识点: do-while 遍历循环链表（必须用 do-while，不能用 while）
//
// 用法: echo "5" | ./34c_clist_print
// 预期输出:
//   1 2 3 4 5

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

// TODO: 实现 clist_print 函数
// 功能: 使用 do-while 循环遍历循环链表，打印每个节点的 int 数据
//       格式: 数字之间用空格分隔，最后一个数字后不加空格，然后换行
//       注意: cur==NULL 时直接返回
//       注意: 循环链表不能用 while(p!=NULL)，要用 do{...}while(p!=start)
// void clist_print(link cur) { ... }

int main(void)
{
    int n;
    scanf("%d", &n);

    link cur = NULL;
    for (int i = 1; i <= n; i++) {
        link item = make_node(make_int_data(i));
        cur = clist_insert_after(cur, item);
    }

    // TODO: 让 cur 指向第一个节点（即最后插入节点的 next）
    // TODO: 调用 clist_print(cur) 打印链表

    return 0;
}
