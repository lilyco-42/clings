/* 42_snprintf.c — snprintf 格式化安全
 *
 * 任务: 1. 实现 is_truncated(): 用 snprintf 向 buf[10] 写入 "Count: %d",
 *          判断是否发生截断，通过 ret 参数返回 snprintf 的返回值
 *       2. 编写 main(): 读整数 n，调用 is_truncated，打印四行结果
 *
 * 核心知识点: snprintf 返回值 = 企图写入的长度（不含 \0）
 *             ≠ 实际截断后写入的长度！
 *   "Count: 100" 共 10 字符，buf[10] 只能装 9 字符 + \0
 *   → 返回值 = 10（想写10个），strlen(buf) = 9（只写了9个）
 *
 * 为什么大厂严禁 sprintf？→ 无边界检查，缓冲区溢出
 *
 * 验证：
 *   stdin: "100\n"   → buf: 'Count: 10'  len: 9  ret: 10  truncated: yes
 *   stdin: "5\n"     → buf: 'Count: 5'   len: 8  ret: 8   truncated: no
 *   stdin: "99999\n" → buf: 'Count: 99'  len: 9  ret: 11  truncated: yes
 */
#include <stdio.h>
#include <string.h>

/* 用 snprintf 格式化写入，检测是否截断
 * buf: 缓冲区
 * size: 缓冲区大小
 * n: 要格式化的整数
 * ret 返回 snprintf 的返回值（企图写入字符数）
 * 函数返回 1=截断 0=完整 */
int is_truncated(char *buf, int size, int n, int *ret) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 调用 snprintf(buf, size, "Count: %d", n)，把返回值存到 *ret */

    /* 如果 *ret >= size：说明输出被截断了
     * （注意和长度区分的细微语义：ret 返回的是完整输出需要的字符数） */

    /* 返回截断标志 */
}

int main(void) {
    char line[32];
    fgets(line, sizeof(line), stdin);
    int n;
    sscanf(line, "%d", &n);

    char buf[10];
    int ret, trunc;
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 调用 is_truncated(buf, 10, n, &ret) */

    /* 依次打印：
     *   printf("buf: '%s'\n", buf)
     *   printf("len: %zu\n", strlen(buf))
     *   printf("ret: %d\n", ret)
     *   printf("truncated: %s\n", trunc ? "yes" : "no") */

    return 0;
}
