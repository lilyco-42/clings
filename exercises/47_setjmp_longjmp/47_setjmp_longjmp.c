/* 47_setjmp_longjmp.c — setjmp/longjmp 非局部跳转
 *
 * 任务: 使用 setjmp 在 main 中设锚点
 *       在深层函数 funcC 中通过 longjmp 跳回 main
 *       理解跨越函数栈的跳转机制
 *
 * 机制: setjmp 保存 CPU 寄存器上下文（SP、PC 等）到 jmp_buf
 *       longjmp 恢复这些寄存器 → 直接"空降"回 setjmp 位置
 *       setjmp 返回 0 = 初次设置；返回非 0 = 从 longjmp 跳回
 *
 * 为什么 C++ 禁用它？（析构函数不会被调用）
 *
 * 知识点: setjmp 双返回值、longjmp 跨栈跳转、jmp_buf、CPU 寄存器上下文
 *
 * 验证：
 *   stdin: "error\n" → caught error: error
 *   stdin: "ok\n"    → funcC: ok  ↓ funcB done  ↓ funcA done  ↓ success: ok
 */
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

jmp_buf env;

/* funcC — 如果输入是 "error"，调用 longjmp 跳回 main */
void funcC(const char *input) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* 如果 !strcmp(input, "error") → longjmp(env, 1)
     *   （这个 longjmp 会导致程序直接跳回 setjmp 的位置，
     *    而且 funcC、funcB、funcA 的剩余代码都不会执行！） */

    /* 正常情况：printf("funcC: %s\n", input) */
}

void funcB(const char *input) {
    funcC(input);
    printf("funcB done\n");
}
void funcA(const char *input) {
    funcB(input);
    printf("funcA done\n");
}

int main(void) {
    char line[64];
    fgets(line, sizeof(line), stdin);
    int len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';

#error TODO: Finish this exercise. Run "clings hint" for help.
    /* int r = setjmp(env)
     * 如果 r == 0（初次设置）：
     *   调用 funcA(line)
     *   打印 success
     * 如果 r != 0（从 longjmp 跳回）：
     *   打印 caught error
     * 注意: 传入 longjmp 的值（这里是 1）会成为 setjmp 的返回值 */

    return 0;
}
