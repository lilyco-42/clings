/*
 * Lesson 22c: 猜数游戏 — 完整 ?A?B
 *
 * 知识点：随机数，数组，函数调用，字符串解析，游戏循环
 * 参考原课 guess.c — 4 位不重复数字猜测游戏
 *
 * 任务：generate_secret 和 check 已实现，学生实现:
 *       1. parse_guess(s, guess): strlen>=4 检查，s[i]-'0' 转换，0-9 范围检查
 *       2. main(): generate_secret, while(1) 循环:
 *          fgets → parse_guess → check → printf("%dA%dB\n")
 *          → 若 a==4 则打印 Congratulations 并 break
 *       3. 最后打印 "The secret was: %d%d%d%d\n"
 *
 * 验证：my_rand() LCG 固定种子，输入 猾测序列
 *       → "0A0B\n1A1B\n4A0B\nCongratulations!...\nThe secret was: 5096\n"
 */

#include <stdio.h>
#include <string.h>

/* 平台无关的伪随机数生成器 (Linear Congruential Generator) */
static unsigned int _seed = 42;
int my_rand(void) { _seed = _seed * 1103515245 + 12345; return (_seed >> 16) & 0x7fff; }

static void generate_secret(int secret[4])
{
	int used[10] = {0};
	for (int i = 0; i < 4; i++) {
		int d;
		do { d = my_rand() % 10; } while (used[d]);
		secret[i] = d;
		used[d] = 1;
	}
}

static void check(const int secret[4], const int guess[4], int *a, int *b)
{
	*a = *b = 0;
	for (int i = 0; i < 4; i++) {
		if (guess[i] == secret[i]) {
			(*a)++;
		} else {
			for (int j = 0; j < 4; j++) {
				if (guess[i] == secret[j]) {
					(*b)++;
					break;
				}
			}
		}
	}
}

static int parse_guess(const char *s, int guess[4])
{
#error TODO: Parse guess string: check length, convert chars to digits. Run "clings hint" for help.
}

int main(void)
{
#error TODO: Implement game loop: generate, read guesses, check, print results. Run "clings hint" for help.

	return 0;
}
