// 33c_array_ops.c — 基于链表实现动态数组操作
//
// 任务: 实现 array_insert(), array_at(), array_length() 函数
//       使用已有的 list.h/list.c 链表接口来实现动态数组
//
// 知识点: 链表封装为数组接口、index 到 node 的映射
//
// 编译: gcc -Wall -Wextra -std=c11 33c_array_ops.c list.c -o test_33c
//
// 预期输出:
//   ['a', 'b', 'c', 'd']
//   length: 4

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef link Array;
typedef link Item;

// TODO: 实现 array_length 函数
// 功能: 从 head->next 开始计数，返回链表中数据节点个数
// int array_length(Array name) { ... }

// TODO: 实现 array_at 函数
// 功能: 返回第 index 个数据节点（0-based），head 算 index=-1
//       遍历链表，当 count==index 时返回当前节点
// Item array_at(Array name, int index) { ... }

// TODO: 实现 array_insert 函数
// 功能: 在 index 位置插入 data
//       特殊情况: index==0 时，插入到 head 之后（即 list_insert_after(name, item)）
//       一般情况: 找到 index-1 位置的节点，在其后插入
// void array_insert(Array name, int index, char data) { ... }

void array_print(Array name)
{
    link p = name->next;
    int first = 1;
    printf("[");
    while (p) {
        if (!first) printf(", ");
        if (p->data)
            printf("'%c'", *(char *)p->data);
        else
            printf("nil");
        first = 0;
        p = p->next;
    }
    printf("]\n");
}

int main(void)
{
    // 创建动态数组（带头节点的链表）
    Array arr = list_new();

    // TODO: 使用 array_insert 插入元素
    // array_insert(arr, 0, 'a');  // ['a']
    // array_insert(arr, 1, 'b');  // ['a', 'b']
    // array_insert(arr, 2, 'c');  // ['a', 'b', 'c']
    // array_insert(arr, 1, 'd');  // ['a', 'd', 'b', 'c']
    //   等等...

    // 以下调用应产生预期输出
    array_insert(arr, 0, 'a');
    array_insert(arr, 1, 'b');
    array_insert(arr, 2, 'c');
    array_insert(arr, 3, 'd');

    array_print(arr);
    printf("length: %d\n", array_length(arr));

    return 0;
}
