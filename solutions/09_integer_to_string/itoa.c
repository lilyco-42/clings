#include <stdio.h>

int main(void)
{
	int num;
	char buf[10];
	int i = 0;
	int j = 0;

	scanf("%d", &num);

	do
	{
		buf[i] = num % 10 + '0';
		i++;

		num /= 10;
	} while (num != 0);
	buf[i] = '\0';

	for (j = 0; j < i / 2; j++)
	{
		char tmp;

		tmp = buf[i - 1 - j];
		buf[i - 1 - j] = buf[j];
		buf[j] = tmp;
	}

	printf("buf = %s\n", buf);

	return 0;
}
