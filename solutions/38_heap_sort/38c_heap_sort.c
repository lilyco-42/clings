/*
 * Lesson 38c: 堆排序 — 参考答案
 */
#include <stdio.h>

#define MAX_SIZE 100

int arr[MAX_SIZE];
int heap_size;
int arr_size;

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

void build_max_heap(void)
{
    for (int i = heap_size / 2 - 1; i >= 0; i--)
        max_heapify(i);
}

void heap_sort(void)
{
    build_max_heap();
    for (int i = arr_size - 1; i >= 1; i--) {
        swap(0, i);
        heap_size--;
        max_heapify(0);
    }
}

void print_array(void)
{
    for (int i = 0; i < arr_size; i++) {
        if (i > 0) printf(" ");
        printf("%d", arr[i]);
    }
    printf("\n");
}

int main(void)
{
    int data[] = {5, 3, 8, 1, 4, 2};
    arr_size = 6;
    heap_size = arr_size;

    for (int i = 0; i < arr_size; i++)
        arr[i] = data[i];

    heap_sort();
    print_array();

    return 0;
}
