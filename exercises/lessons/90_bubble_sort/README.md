## Lesson 90 Project: Compile Bubble Sort 项目课：编译冒泡排序

### 代码

    /* 输入: 至此全部特性的综合体 */
    int main() {
        int a[5];
        int i; int j; int t;
        a[0] = 5; a[1] = 3; a[2] = 4; a[3] = 1; a[4] = 2;
        for (i = 0; i < 4; i = i + 1) {
            for (j = 0; j < 4 - i; j = j + 1) {
                if (a[j] > a[j + 1]) {
                    t = a[j]; a[j] = a[j + 1]; a[j + 1] = t;
                }
            }
        }
        return a[0] * 10000 + a[1] * 1000 + a[2] * 100 + a[3] * 10 + a[4];
    }
    /* 12345 & 255 = 57 — 退出码按 8 位折算 */

### 知识点

- 项目课的意义：特性交叉处最容易藏 bug
  - `a[j + 1]` = 下标里带表达式 → 脱糖 + 缩放 + 栈式求值三者叠加
  - 双重循环 + 嵌套 if → 标号管理与循环上下文栈的压力测试
  - 三变量轮换交换 → 左值/右值路径在一条语句里交替出现
- 差分验证：同一程序喂给 gcc
  - `gcc bubble.c && ./a.out; echo $?` 与 nccl-cc 产物的退出码必须一致
  - "期望值不是拍脑袋，是 gcc 说了算" — 本课程测试体系的根基
- 验证结果编码技巧
  - 把 5 个元素折成一个整数 12345 — 一个退出码验证全部排序结果
  - & 255 的折算：12345 % 256 = 57, 测试头写 `// expect: 57`

### 课堂讨论

- 冒泡的内层边界 `4 - i` 写成 `4` 还能排对吗？多花多少比较？
- 交换三句 t=a[j]; a[j]=a[j+1]; a[j+1]=t; — 每句各生成几次地址计算？`a[j]` 算了几遍？(公共子表达式消除的诱惑)
- 如果想排 1000 个数，退出码验证法失效 — 你有哪些替代方案？

### 课后练习

- 测试：已排序输入 (最好情况), 逆序输入 (最坏情况), 全相等输入
- 改写：用指针遍历替代下标 `*(a+j)` — 输出汇编差异多大？
- 里程碑自查：M4 达成 — 数组 + 指针 + 循环的数据密集程序可编译

### 参考资料

- K&R 习题 — 排序程序家族
- 《编程珠玑》第 11 章 — 排序的工程细节

---

### 本课文件

    test_bubble.c

### 在本仓验证

    cd ../../nccl-cc && make
    ./nccl-cc ../lessons/90_bubble_sort/test_bubble.c > /tmp/t.s
    riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static /tmp/t.s runtime_rv_io.s runtime_rv.s -o /tmp/t
    qemu-riscv32 /tmp/t; echo $?

> 课文源：[NCCL/Unit-4/Lesson-90.md](https://cnb.cool/opencamp/learning-nccl/NCCL/-/blob/master/Unit-4/Lesson-90.md) — 如有更新以书仓为准
