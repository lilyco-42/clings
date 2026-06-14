#include <stdio.h>
#include <math.h>

struct point
{
	float x;
	float y;
};

typedef struct point point_t;

float calculate(point_t p1, point_t p2)
{
	float dx = p1.x - p2.x;
	float dy = p1.y - p2.y;

	return sqrt(dx * dx + dy * dy);
}

int main(void)
{
	point_t p1, p2;
	float distance;

	scanf("%f %f %f %f", &p1.x, &p1.y, &p2.x, &p2.y);

	distance = calculate(p1, p2);
	printf("%.2f\n", distance);

	return 0;
}
