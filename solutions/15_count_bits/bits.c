#include <stdio.h>

#define M1	0x55555555
#define M2	0x33333333
#define M3	0x0F0F0F0F
#define M4	0x00FF00FF
#define M5	0x0000FFFF

int count_bits(int num)
{
	int sum = num;

	sum = (sum & M1) + ((sum >> 1) & M1);
	sum = (sum & M2) + ((sum >> 2) & M2);
	sum = (sum & M3) + ((sum >> 4) & M3);
	sum = (sum & M4) + ((sum >> 8) & M4);
	sum = (sum & M5) + ((sum >> 16) & M5);

	return sum;
}

int main(void)
{
	int num;

	scanf("%d", &num);
	printf("%d\n", count_bits(num));

	return 0;
}
