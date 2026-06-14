#include <stdio.h>

char *itoa_hex(int num, char *buf)
{
	char *hex = "0123456789ABCDEF";
	int i = 0;
	int j;

	do
	{
		int rest;

		rest = num % 16;
		buf[i++] = hex[rest];
		num /= 16;
	} while (num != 0);

	buf[i] = '\0';

	for (j = 0; j < i / 2; j++)
	{
		char tmp;
		tmp = buf[j];
		buf[j] = buf[i - 1 - j];
		buf[i - 1 - j] = tmp;
	}

	return buf;
}

int main(void)
{
	int num;
	char buf[64];

	scanf("%d", &num);

	itoa_hex(num, buf);
	printf("%s\n", buf);

	return 0;
}
