/*
 * Lesson 11: 求两个坐标点之间的距离
 *
 * 知识点：struct, typedef, 函数参数传结构体，math.h sqrt
 *
 * 任务：参考原课 point.c，完成:
 *       1. 定义 struct point { float x; float y; } 和 typedef
 *       2. 实现 float calculate(point_t p1, point_t p2)
 *          - 计算 dx = p1.x - p2.x, dy = p1.y - p2.y
 *          - return sqrt(dx*dx + dy*dy)
 *       3. main 中 scanf 读入两点坐标，调用函数，打印结果
 *
 * 验证：输入 "0 0 3 4" → "5.00\n"; 输入 "1 1 4 5" → "5.00\n"
 */

#include <stdio.h>
#include <math.h>

#error TODO: Define struct point {float x; float y;} with typedef. Run "clings hint" for help.

#error TODO: Implement float calculate(point_t p1, point_t p2) for distance. Run "clings hint" for help.

int main(void)
{
	point_t p1, p2;
	float distance;

	scanf("%f %f %f %f", &p1.x, &p1.y, &p2.x, &p2.y);

	distance = calculate(p1, p2);
	printf("%.2f\n", distance);

	return 0;
}
