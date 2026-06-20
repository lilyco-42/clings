/*
 * Lesson 33c: 基于链表的动态数组 — 参考答案
 */
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

typedef link Array;
typedef link Item;

int array_length(Array name)
{
    link p = name;
    int count = 0;
    while (p->next != NULL) {
        p = p->next;
        count++;
    }
    return count;
}

Item array_at(Array name, int index)
{
    Item p = name;
    int count = -1;
    while (p != NULL) {
        if (count == index)
            return p;
        p = p->next;
        count++;
    }
    return NULL;
}

void array_insert(Array name, int index, char data)
{
    char *p = make_data(data);
    link item = make_node(p);
    if (index == 0) {
        list_insert_after(name, item);
        return;
    }
    Item where = array_at(name, index - 1);
    list_insert_after(where, item);
}

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
    Array arr = list_new();

    array_insert(arr, 0, 'a');
    array_insert(arr, 1, 'b');
    array_insert(arr, 2, 'c');
    array_insert(arr, 3, 'd');

    array_print(arr);
    printf("length: %d\n", array_length(arr));

    return 0;
}
