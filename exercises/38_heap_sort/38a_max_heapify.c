// 38a_max_heapify.c — 最大堆调整: 单节点下沉
//
// 任务: 实现 max_heapify 函数，对指定位置做堆调整
//       数组 [4,10,3,5,1]，对 index 0 做 heapify
//
// 知识点: 堆的数组表示、父子节点下标关系
//   - 左子节点: 2*i + 1
//   - 右子节点: 2*i + 2
//   - 比较父与子，将最大值交换到父位置
//
// 预期输出:
//   10 5 3 4 1

#include <stdio.h>

#define MAX_SIZE 100

int arr[MAX_SIZE];
int heap_size;

// TODO: 实现 swap 函数
// 功能: 交换 arr[a] 和 arr[b]
// void swap(int a, int b) { ... }

// TODO: 实现 max_heapify 函数
// 功能: 对 arr[i] 做最大堆调整（假设左右子树已经是最大堆）
//   1. 计算 left = 2*i + 1, right = 2*i + 2
//   2. 在 i, left, right 中找最大值的下标 largest
//   3. 如果 largest != i，交换并递归 max_heapify(largest)
// void max_heapify(int i) { ... }

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

    // TODO: 调用 max_heapify(0)
    // TODO: 调用 print_array()

    return 0;
}
