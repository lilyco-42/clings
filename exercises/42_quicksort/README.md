## Lesson 42 - QSort 实现 (分治法)

### 课程任务

快速排序采用分治法（Divide-and-Conquer），是实际应用中最常用的排序算法之一。本课实现两种分区方案 (Lomuto/Hoare) 和随机化优化。

### 代码

    /* Lomuto 分区: pivot = 最后一个元素 */
    static int partition_lomuto(int arr[], int low, int high)
    {
        int pivot = arr[high];
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (arr[j] <= pivot)
                swap(&arr[++i], &arr[j]);
        }
        swap(&arr[i + 1], &arr[high]);
        return i + 1;
    }

    /* 随机化: 避免已排序输入的 O(n²) 最坏情况 */
    void quicksort_random(int arr[], int low, int high)
    {
        if (low < high) {
            int r = low + rand() % (high - low + 1);
            swap(&arr[r], &arr[high]);  /* 随机选 pivot */
            int p = partition_lomuto(arr, low, high);
            quicksort_random(arr, low, p - 1);
            quicksort_random(arr, p + 1, high);
        }
    }

### 重要知识点

- 分区 (Partition) — 快排的核心
  - **Lomuto**: pivot 取末尾，i 指针扫描，交换次数较多但逻辑简单
  - **Hoare**: pivot 取首部，双指针相向扫描，平均交换次数少 ~3x
  - 分区结束后：pivot 左边全 ≤ pivot，右边全 ≥ pivot
- 最坏情况与随机化
  - 已排序数组 + 首/尾 pivot = O(n²) 退化
  - 随机选 pivot: 期望 O(n log n)，避免恶意输入
- 分治法思想

在 [《C 程序设计语言》](https://www.dropbox.com/s/qer3va6rtq8o1dj/C%E7%A8%8B%E5%BA%8F%E8%AE%BE%E8%AE%A1%E8%AF%AD%E8%A8%80%EF%BC%88%E7%AC%AC2%E7%89%88%C2%B7%E6%96%B0%E7%89%88%EF%BC%89.pdf)书中，介绍了一个快速排序函数 qsort()，如下所示：

    /* swap: interchange v[i] and v[j] */
    void swap(int v[], int i, int j)
    {
    	int temp;
    	temp = v[i];
    	v[i] = v[j];
    	v[j] = temp;
    }

    /* qsort: sort v[left]...v[right] into increasing order */
    void qsort(int v[], int left, int right)
    {
    	int i, last;
    	void swap(int v[], int i, int j);
    	if (left >= right) /* do nothing if array contains */
    		return; /* fewer than two elements */

    	swap(v, left, (left + right)/2); /* move partition elem */
    	last = left; /* to v[0] */
    	for (i = left + 1; i <= right; i++) /* partition */
    		if (v[i] < v[left])
    			swap(v, ++last, i);
    	swap(v, left, last);
    	qsort(v, left, last-1);
    	qsort(v, last+1, right);
    	/* restore partition elem */
    }

请参照这个算法思想，实现标准库函数中 qsort 函数。

    #include <stdlib.h>

    void
    qsort(void *base, size_t nel, size_t width,
    		int (*compar)(const void *, const void *));

### 课堂讨论

- Lomuto 和 Hoare 分区各有什么优缺点？
- 为什么已排序数组是 naive quicksort 的最坏情况？
- 三路快排 (Dutch National Flag) 对大量重复元素有什么优势？

### 课后练习

- 测试：对 `{5,3,9,1,7,2,8,4,6}` 排序
- 对比：随机化 vs 非随机化在已排序输入上的递归深度
- 挑战：实现泛型 `qsort(void *base, ...)` 与标准库接口一致

### 参考资料

- K&R《C 程序设计语言》第 4.10 节 递归
- 快速排序详解 http://blog.csdn.net/wzy_1988/article/details/8043168
- Sedgewick "Quicksort is Optimal" https://www.cs.princeton.edu/~rs/talks/QuicksortIsOptimal.pdf
