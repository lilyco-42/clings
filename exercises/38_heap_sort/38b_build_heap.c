// 38b_build_heap.c — 构建最大堆: 自底向上建堆
//
// 任务: 实现 build_max_heap 函数
//       数组 [4,1,3,2,16,9,10,14,8,7]，建堆后打印
//
// 知识点: 从最后一个非叶节点开始，逐个向上做 max_heapify
//   - 最后一个非叶节点下标: heap_size/2 - 1
//   - 自底向上保证子树先满足堆性质
//
// 预期输出:
//   16 14 10 8 7 9 3 2 4 1

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

// TODO: 实现 build_max_heap 函数
// 功能: 从 heap_size/2 - 1 到 0，逐个调用 max_heapify
//   for (int i = heap_size/2 - 1; i >= 0; i--)
//       max_heapify(i);
// void build_max_heap(void) { ... }

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
    int data[] = {4, 1, 3, 2, 16, 9, 10, 14, 8, 7};
    heap_size = 10;

    for (int i = 0; i < heap_size; i++)
        arr[i] = data[i];

    // TODO: 调用 build_max_heap()
    // TODO: 调用 print_array()

    return 0;
}
