// 33a_make_node.c — 链表节点创建与释放
//
// 任务: 实现 make_node() 和 delete_node() 函数
//       使用 malloc 分配节点，存储 data 指针，用完后 free 释放
//
// 知识点: malloc/free、结构体指针、void* 通用数据
//
// 预期输出:
//   A
//   B
//   C

#include <stdio.h>
#include <stdlib.h>

struct node {
    void *data;
    struct node *next;
};

typedef struct node *link;

// TODO: 实现 make_node 函数
// 功能: 分配一个 struct node，设置 data 字段，next 设为 NULL，返回指针
// link make_node(void *data) { ... }

// TODO: 实现 delete_node 函数
// 功能: 释放节点的 data，再释放节点本身
// void delete_node(link p) { ... }

int main(void)
{
    // 创建三个字符数据
    char *a = malloc(sizeof(char));
    char *b = malloc(sizeof(char));
    char *c = malloc(sizeof(char));
    *a = 'A';
    *b = 'B';
    *c = 'C';

    // TODO: 用 make_node 创建三个节点 n1, n2, n3
    // TODO: 打印每个节点的数据: printf("%c\n", *(char *)n->data);
    // TODO: 用 delete_node 释放三个节点

    return 0;
}
