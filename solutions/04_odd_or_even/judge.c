#include <stdio.h>

int main(void)
{
	int num;

	scanf("%d", &num);

	if (num % 2 == 0)
		printf("num %d is even\n", num);
	else
		printf("num %d is odd\n", num);

	return 0;
}
