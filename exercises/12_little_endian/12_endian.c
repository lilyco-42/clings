/*
 * Lesson 12: 判断大小端
 *
 * 知识点：union, sizeof, 字节序 (little-endian vs big-endian)
 *
 * 任务：参考原课 union.c，用 union 检测字节序:
 *       1. 定义 union { unsigned char c[4]; int i; } u
 *       2. 设置 u.i = 1
 *       3. 如果 u.c[0] == 1，说明低字节在前 → 小端 → return 1
 *       4. 否则大端 → return 0
 *
 * 课堂讨论：为什么 union 的 sizeof 等于最大成员的 sizeof?
 *
 * 验证：clings 检查返回值是否为 1 (x86/ARM 均为小端)
 */

union endian_test
{
	unsigned char c[4];
	int i;
};

int main(void)
{
	union endian_test u;

#error TODO: Fix this exercise. Run "clings hint" for help.

	return 0;
}
