/*
 * Lesson 38a: 最大堆调整 — 参考答案
 */
#include <stdio.h>

#define MAX_SIZE 100

int arr[MAX_SIZE];
int heap_size;

void swap(int a, int b)
{
    int tmp = arr[a];
    arr[a] = arr[b];
    arr[b] = tmp;
}

void max_heapify(int i)
{
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    int largest = i;

    if (left < heap_size && arr[left] > arr[largest])
        largest = left;
    if (right < heap_size && arr[right] > arr[largest])
        largest = right;

    if (largest != i) {
        swap(i, largest);
        max_heapify(largest);
    }
}

void print_array(void)
{
    for (int i = 0; i < heap_size; i++) {
        if (i > 0) printf(" ");
        printf("%d", arr[i]);
    }
    printf("\n");
}

int main(void)
{
    int data[] = {4, 10, 3, 5, 1};
    heap_size = 5;

    for (int i = 0; i < heap_size; i++)
        arr[i] = data[i];

    max_heapify(0);
    print_array();

    return 0;
}
