// 33b_insert_tail.c — 链表尾插法
//
// 任务: 实现 list_insert_tail() 函数
//       从头遍历到尾部，将新节点挂到末尾
//
// 知识点: 链表尾部遍历、尾插法、带头节点的链表
//
// 预期输出:
//   x -> y -> z

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
// 功能: 从 head 开始遍历到最后一个节点（p->next == NULL），
//       将 item 挂到尾部，item->next 设为 NULL，返回 head
// link list_insert_tail(link head, link item) { ... }

void print_char(void *data)
{
    if (data)
        printf("%c", *(char *)data);
}

int main(void)
{
    // 创建带头节点的链表（头节点 data 为 NULL）
    link head = make_node(NULL);

    // 创建数据节点
    char *x = malloc(1); *x = 'x';
    char *y = malloc(1); *y = 'y';
    char *z = malloc(1); *z = 'z';

    // TODO: 调用 list_insert_tail 将三个节点依次插入链表
    // TODO: 从 head->next 开始遍历打印，元素之间用 " -> " 分隔，最后换行
    //   示例:
    //     link p = head->next;
    //     while (p) {
    //         if (p != head->next) printf(" -> ");
    //         print_char(p->data);
    //         p = p->next;
    //     }
    //     printf("\n");

    return 0;
}
