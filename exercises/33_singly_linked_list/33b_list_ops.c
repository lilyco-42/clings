// 33b_list_ops.c — 链表尾插与遍历打印
//
// 任务: 实现 list_insert_tail() 和 list_print() 函数
//       构建链表并用函数指针回调打印每个元素
//
// 知识点: 链表尾插遍历、函数指针、带头节点的链表
//
// 预期输出:
//   a -> b -> c

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

// TODO: 实现 list_insert_tail 函数
// 功能: 找到链表尾部（从 head 开始遍历到 next==NULL 的节点），
//       将 item 挂到尾部，返回 head
// link list_insert_tail(link head, link item) { ... }

// TODO: 实现 list_print 函数
// 功能: 从 head->next 开始遍历（跳过头节点），
//       对每个节点调用 pf(p->data)，最后打印换行 '\n'
//       注意: 元素之间用 " -> " 分隔，最后一个元素后不加分隔符
// void list_print(link head, void (*pf)(void *)) { ... }

void print_char(void *data)
{
    if (data)
        printf("%c", *(char *)data);
}

int main(void)
{
    // 创建带头节点的链表
    link head = make_node(NULL);

    // 创建数据节点并尾插
    char *a = malloc(1); *a = 'a';
    char *b = malloc(1); *b = 'b';
    char *c = malloc(1); *c = 'c';

    // TODO: 调用 list_insert_tail 将三个节点插入链表
    // TODO: 调用 list_print(head, print_char) 打印链表

    return 0;
}
