/* 26_my_memcpy.c — 实现 my_memcpy：标准内存拷贝
 *
 * 任务: 实现 my_memcpy(dest, src, n) — 逐字节拷贝
 *       将 n 字节从 src 拷贝到 dest，返回 dest
 *       掌握 void* 到 char* 的转换与逐字节拷贝的底层逻辑
 *
 * 知识点: void* 泛型指针、char* 逐字节操作、size_t 类型
 *         为后续 my_memmove（内存重叠拷贝）打底
 *
 * 验证：
 *   stdin: "hello\n"  → hello
 *   stdin: "abc\n"    → abc
 *   stdin: "\n"       → (空行)
 */
#include <stdio.h>
#include <string.h>

/* 实现 my_memcpy — void* 转为 char*，逐字节拷贝 */
void *my_memcpy(void *dest, const void *src, size_t n) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 将 void* 转为 char*，这样才能逐字节操作 */

    /* 逐字节拷贝：循环 n 次，每次拷贝一个字节 */

    /* 返回 dest */
}

int main(void) {
    char src[256], dest[256] = {0};
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 用 fgets 读一行到 src，去掉末尾的 '\n' */

    /* 调用 my_memcpy 将 src 拷贝到 dest（包括结尾 '\0'）。
     * 提示：要拷贝的字节数 = strlen(src) + 1（+1 是把 '\0' 也拷过去） */

    /* printf 打印 dest，验证拷贝结果正确 */
    return 0;
}
