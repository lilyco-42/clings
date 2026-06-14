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
	if (strlen(s) < 4) return -1;
	for (int i = 0; i < 4; i++) {
		if (s[i] < '0' || s[i] > '9') return -1;
		guess[i] = s[i] - '0';
	}
	return 0;
}

int main(void)
{
	int secret[4];
	generate_secret(secret);

	char input[64];
	int attempts = 0;

	while (1) {
		if (!fgets(input, sizeof(input), stdin)) break;
		input[strcspn(input, "\r\n")] = '\0';

		int guess[4];
		if (parse_guess(input, guess) < 0)
			continue;

		int a, b;
		check(secret, guess, &a, &b);
		attempts++;
		printf("%dA%dB\n", a, b);

		if (a == 4) {
			printf("Congratulations! You got it in %d attempts!\n", attempts);
			break;
		}
	}

	printf("The secret was: %d%d%d%d\n", secret[0], secret[1], secret[2], secret[3]);
	return 0;
}
