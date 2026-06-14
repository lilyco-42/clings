#include <stdio.h>

int find(int num, int digit)
{
	int counter = 0;

	do {
		if (num % 10 == digit)
			counter++;
		num = num / 10;
	} while (num != 0);

	return counter;
}

int main(void)
{
	int sum = 0;

	for (int i = 1; i <= 100; i++)
		sum += find(i, 9);

	printf("sum = %d\n", sum);
	return 0;
}
