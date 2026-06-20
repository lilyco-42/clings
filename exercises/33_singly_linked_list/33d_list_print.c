// 33d_list_print.c — 函数指针遍历打印链表
//
// 任务: 实现 list_print() 函数
//       使用函数指针回调实现泛型打印（void* 数据 + 回调负责类型转换）
//
// 知识点: 函数指针、void* 泛型、回调函数
//
// 预期输出:
//   10 20 30 

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

link list_insert_tail(link head, link item)
{
    link p = head;
    while (p->next != NULL)
        p = p->next;
    p->next = item;
    item->next = NULL;
    return head;
}

// TODO: 实现 list_print 函数
// 功能: 从 head->next 开始遍历（跳过哨兵头节点），
//       对每个节点调用 pf(p->data)，最后打印换行 '\n'
// void list_print(link head, void (*pf)(void *)) { ... }

// 回调函数: 将 void* 转为 int* 并打印
void print_int(void *data)
{
    printf("%d ", *(int *)data);
}

int main(void)
{
    // 创建带头节点的链表
    link head = make_node(NULL);

    // 创建 int 数据节点
    int *a = malloc(sizeof(int)); *a = 10;
    int *b = malloc(sizeof(int)); *b = 20;
    int *c = malloc(sizeof(int)); *c = 30;

    list_insert_tail(head, make_node(a));
    list_insert_tail(head, make_node(b));
    list_insert_tail(head, make_node(c));

    // TODO: 调用 list_print(head, print_int) 打印链表

    return 0;
}
