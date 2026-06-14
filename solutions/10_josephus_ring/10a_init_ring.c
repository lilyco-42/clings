#include <stdio.h>

#define MAX 1000

int next[MAX];
int n;

void init_ring(void)
{
	int i;

	for (i = 0; i < n; i++)
		next[i] = (i + 1) % n;
}

void print_ring(void)
{
	int i;

	for (i = 0; i < n; i++)
		printf("%d ", next[i]);
	printf("\n");
}

int main(void)
{
	scanf("%d", &n);

	init_ring();
	print_ring();

	return 0;
}
