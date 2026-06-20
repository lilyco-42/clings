// 38c_heap_sort.c — 堆排序: 完整实现
//
// 任务: 实现完整的堆排序算法
//       对数组 [5,3,8,1,4,2] 排序后打印
//
// 知识点: 堆排序步骤:
//   1. build_max_heap: 将数组建成最大堆
//   2. 循环: 将堆顶（最大值）与末尾交换，缩小堆大小，再 heapify
//
// 预期输出:
//   1 2 3 4 5 8

#include <stdio.h>

#define MAX_SIZE 100

int arr[MAX_SIZE];
int heap_size;
int arr_size;  /* 保存原始数组大小用于最终打印 */

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

// TODO: 实现 heap_sort 函数
// 功能:
//   1. 调用 build_max_heap()
//   2. 从数组末尾到 1:
//      a. swap(0, i) — 将当前最大值放到末尾
//      b. heap_size-- — 缩小堆范围
//      c. max_heapify(0) — 恢复堆性质
// void heap_sort(void) { ... }

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

    // TODO: 调用 heap_sort()
    // TODO: 调用 print_array()

    return 0;
}
