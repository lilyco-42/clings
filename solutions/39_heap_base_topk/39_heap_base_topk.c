/* 39_heap_base_topk.c — 堆与Top-K（参考解答） */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void heapify(int arr[], int n, int i) {
    int smallest = i, l = 2 * i + 1, r = 2 * i + 2;
    if (l < n && arr[l] < arr[smallest]) smallest = l;
    if (r < n && arr[r] < arr[smallest]) smallest = r;
    if (smallest != i) {
        swap(&arr[i], &arr[smallest]);
        heapify(arr, n, smallest);
    }
}
void build_heap(int arr[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);
}
void top_k(int arr[], int n, int k) {
    int *heap = malloc(k * sizeof(int));
    for (int i = 0; i < k; i++) heap[i] = arr[i];
    build_heap(heap, k);
    for (int i = k; i < n; i++)
        if (arr[i] > heap[0]) {
            heap[0] = arr[i];
            heapify(heap, k, 0);
        }
    for (int i = k - 1; i > 0; i--) {
        swap(&heap[0], &heap[i]);
        heapify(heap, i, 0);
    }
    for (int i = 0; i < k; i++) {
        if (i > 0) printf(" ");
        printf("%d", heap[i]);
    }
    printf("\n");
    free(heap);
}

int main(void) {
    char line1[1024], line2[32];
    fgets(line1, sizeof(line1), stdin);
    fgets(line2, sizeof(line2), stdin);
    int arr[1024], n = 0, k;
    char *tok = strtok(line1, " \n");
    while (tok) {
        arr[n++] = atoi(tok);
        tok = strtok(NULL, " \n");
    }
    sscanf(line2, "%d", &k);
    top_k(arr, n, k);
    return 0;
}
