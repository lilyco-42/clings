/* 27_my_memmove.c — 实现 my_memmove：内存重叠拷贝 【重点】
 *
 * 任务: 实现 my_memmove(dest, src, n)
 *       将 n 字节从 src 拷贝到 dest，返回 dest
 *       与 memcpy 不同：必须正确处理 src 与 dest 内存重叠的情况
 *       考察源与目的地址前后关系的逻辑判断
 *
 * 知识点: 内存重叠、正向/反向拷贝、void* 到 char* 转换、指针比较（d < s）
 *
 * 算法要点:
 *   - 当 dest < src：正向拷贝（从前往后），安全
 *   - 当 dest > src：反向拷贝（从后往前），否则正向会覆盖尚未读取的数据
 *
 * 验证：
 *   stdin: "abcd\n"   → aabc   （dest > src 场景，验证反向拷贝）
 *   stdin: "hello\n"  → hhell
 *   stdin: "xy\n"     → xx
 */
#include <stdio.h>
#include <string.h>

/* 实现 my_memmove — 判断 dest 与 src 的位置关系 */
void *my_memmove(void *dest, const void *src, size_t n) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 将 dest 和 src 转为 char* 指针，方便逐字节操作 */

    /* 如果 dest == src 或 n == 0，直接返回 dest（无需拷贝） */

    /* 情况 1：dest < src（目标在源左边，无重叠风险）
     *        正向拷贝——从前往后 */

    /* 情况 2：dest > src（目标在源右边，有重叠风险！）
     *        必须反向拷贝——从后往前
     *        想一想：如果正向会发生什么？后面的源数据被前面写入的目标覆盖了 */
}

int main(void) {
    char buf[256];
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 用 fgets 读一行到 buf，去掉末尾 '\n' */

    /* 计算字符串长度 len = strlen(buf) */

    /* 核心测试：将 buf[0..len-2] 拷贝到 buf[1..len-1]
     * 即相当于把整个字符串往右挪一位：
     *   "abcd" → "aabc"（第 2 个字符起是原来的前 3 个字符）
     * 这是 dest > src 的典型场景——必须用反向拷贝 */

    /* 打印 buf */
    return 0;
}
