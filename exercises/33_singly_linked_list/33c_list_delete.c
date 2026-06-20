// 33c_list_delete.c — 链表节点删除
//
// 任务: 实现 list_delete() 函数
//       找到被删节点的前驱，让前驱跳过被删节点
//
// 知识点: prev 指针技巧、free 释放、带头节点简化删除
//
// 预期输出:
//   a -> c

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

// TODO: 实现 list_delete 函数
// 功能: 从 head 开始找前驱节点 p，使得 p->next == item
//       找到后: p->next = item->next; free(item->data); free(item);
//       返回 head
// link list_delete(link head, link item) { ... }

void print_char(void *data)
{
    if (data)
        printf("%c", *(char *)data);
}

int main(void)
{
    // 创建带头节点的链表
    link head = make_node(NULL);

    // 构建链表: a -> b -> c
    char *a = malloc(1); *a = 'a';
    char *b = malloc(1); *b = 'b';
    char *c = malloc(1); *c = 'c';

    link na = make_node(a);
    link nb = make_node(b);
    link nc = make_node(c);

    list_insert_tail(head, na);
    list_insert_tail(head, nb);
    list_insert_tail(head, nc);

    // TODO: 删除中间节点 nb
    // TODO: 从 head->next 开始遍历打印，元素之间用 " -> " 分隔，最后换行

    return 0;
}
