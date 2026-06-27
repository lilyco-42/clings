/* 39_heap_base_topk.c — 堆与Top-K问题
 *
 * 任务: 实现小顶堆，解决 Top-K 问题
 *       1. heapify(arr, n, i) — 对节点 i 做向下调整（小顶堆：根最小）
 *       2. build_heap(arr, n) — 建堆（自底向上 heapify）
 *       3. top_k(arr, n, k)  — 输出数组中最大的 k 个数（降序）
 *
 * 核心思路 — 小顶堆维护 K 个最大值:
 *       堆顶 = K 个数中最小的 = "门槛"
 *       新元素 ≥ 门槛 ? 替换堆顶并调整 : 丢弃
 *       最后 K 个数就是最大的 K 个
 *
 * 知识点: 堆的数组表示(2i+1, 2i+2)、向下调整、Top-K 用小顶堆（不是大顶堆！）
 *
 * 验证：
 *   stdin: "5 3 8 1 9 2\n3\n" → 9 8 5  (最大的3个，降序)
 *   stdin: "5 3 8 1 9 2\n1\n" → 9       (最大的1个)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

/* 小顶堆下沉：确保以 i 为根的子树满足堆性质
 * 比较 i 和它的左右孩子，如果 i 不是最小的，与较小的孩子交换 */
void heapify(int arr[], int n, int i) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 假设 i 是最小的：smallest = i; l = 2*i+1; r = 2*i+2 */

    /* 如果左孩子在范围内且比 arr[smallest] 小 → smallest = l */

    /* 如果右孩子在范围内且比 arr[smallest] 小 → smallest = r */

    /* 如果 smallest != i：交换 arr[i] 和 arr[smallest]
     *   递归 heapify(arr, n, smallest) 继续下沉 */
}

/* 建堆：从最后一个非叶节点开始（n/2 - 1），向前逐个 heapify
 * 自底向上保证每个子树都是合法的小顶堆 */
void build_heap(int arr[], int n) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* for (i = n/2 - 1; i >= 0; i--) heapify(arr, n, i) */
}

/* Top-K：用大小为 k 的小顶堆维护最大的 k 个数
 * 1. 取前 k 个元素建堆
 * 2. 遍历剩余元素：如果当前元素 > 堆顶（门槛），替换堆顶并下沉
 * 3. 堆中元素从大到小排序输出
 * 注意：需要动态分配大小为 k 的堆数组 */
void top_k(int arr[], int n, int k) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* malloc 一个大小为 k 的堆数组 */

    /* 将 arr 的前 k 个元素拷入堆中 */

    /* build_heap(heap, k) */

    /* 遍历剩余元素 i = k..n-1：
     *   如果 arr[i] > heap[0]（比门槛大）：
     *     heap[0] = arr[i]; heapify(heap, k, 0) */

    /* 堆中元素排序输出（从大到小）：
     *   for (i = k-1; i > 0; i--) { swap; heapify(heap, i, 0) }
     *   然后从后往前打印 k 个元素 */

    /* free(heap) */
}

int main(void) {
    char line1[1024], line2[32];
    fgets(line1, sizeof(line1), stdin);
    fgets(line2, sizeof(line2), stdin);

    int arr[1024], n = 0, k;

    /* 解析第一行数字 */
    char *tok = strtok(line1, " \n");
    while (tok) {
        arr[n++] = atoi(tok);
        tok = strtok(NULL, " \n");
    }

    sscanf(line2, "%d", &k);
    top_k(arr, n, k);
    return 0;
}
