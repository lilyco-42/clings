/*
 * Lesson 02b: 格式化输出
 *
 * 知识点：格式化输出 %d %x %p, 全局变量，局部变量，取地址符 &
 *
 * 任务：参考原课 print.c 的风格，完成以下程序:
 *       1. 定义全局变量 int global = 200
 *       2. 在 main 中定义 int local = 100
 *       3. 用 printf 打印两行:
 *          local = 100         (用 %d 格式)
 *          global = 0xc8       (用 %x 格式，注意小写十六进制)
 *
 * 课堂讨论：%x 和 %p 在打印地址时，有何区别？
 *           尝试加一行 printf("&local = %p\n", &local); 观察地址
 *
 * 验证：stdout 精确匹配 "local = 100\nglobal = 0xc8\n"
 */

#include <stdio.h>

#error TODO: Fix this exercise. Run "clings hint" for help.

int main(void)
{
#error TODO: Fix this exercise. Run "clings hint" for help.
	return 0;
}
