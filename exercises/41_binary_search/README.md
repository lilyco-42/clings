## Lesson 41 - BSearch 实现 (二分查找法)

### 课程任务

实现多种二分查找变体，掌握二分查找的正确写法和常见陷阱。

### 代码

    /* 迭代版: 用 low + (high-low)/2 避免整数溢出 */
    int bsearch_iter(int x, const int v[], int n)
    {
        int low = 0, high = n - 1;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            if (v[mid] == x) return mid;
            else if (v[mid] < x) low = mid + 1;
            else high = mid - 1;
        }
        return -1;
    }

    /* lower_bound: 找第一个 >= x 的位置 (插入位置) */
    int lower_bound(int x, const int v[], int n)
    {
        int low = 0, high = n;
        while (low < high) {
            int mid = low + (high - low) / 2;
            if (v[mid] < x) low = mid + 1;
            else high = mid;
        }
        return low;
    }

    /* 泛型版: 通过函数指针比较任意类型 */
    int bsearch_generic(const void *key, const void *base,
                        int count, int size,
                        int (*cmp)(const void *, const void *));

### 重要知识点

- 二分查找的正确性
  - `mid = low + (high - low) / 2` 避免 `(low+high)` 溢出
  - 循环不变量：答案始终在 `[low, high]` 区间内
  - 终止条件：`low > high` (查找) 或 `low >= high` (lower_bound)
- 四种变体
  - **迭代版**: 最基本，O(log n) 时间 O(1) 空间
  - **递归版**: 代码更简洁，但有栈开销
  - **lower_bound**: 找"第一个不小于 x 的位置" — 插入排序、去重的基础
  - **泛型版**: 用 `void *` + 比较函数，可搜索任何类型
- 泛型编程 (void\* + comparator)
  - `const void *base + mid * size` 计算元素地址
  - 与标准库 `bsearch()` 接口一致

### 课堂讨论

- `low <= high` 和 `low < high` 作为循环条件有什么区别？
- 为什么标准库的 `bsearch` 要求数组已排序？如果未排序会怎样？
- lower_bound 和 upper_bound 有什么区别？各自用于什么场景？

### 课后练习

- 测试：在 {2,5,8,12,16,23,38,42,56,72,91} 中查找 50 (应返回 -1)
- 实现 `upper_bound`: 找第一个 > x 的位置
- 挑战：用二分查找实现 `sqrt(n)` 的整数版本

### 参考资料

- K&R《C 程序设计语言》第 3.3 节 二分查找
- 把二分查找算法写正确需要注意的地方 http://www.cppblog.com/converse/archive/2014/01/28/96893.html
- 二分查找在数据库内核中的实现 http://hedengcheng.com/?p=595
