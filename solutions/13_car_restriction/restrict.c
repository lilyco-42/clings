#include <stdio.h>

enum day
{
	MONDAY = 1,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY,
	SUNDAY
};

char get_last_char(char str[])
{
	char c = 0;
	int i = 0;

	while (str[i])
	{
		c = str[i];
		i++;
	}
	return c;
}

int is_restricted(int tail_num, enum day today)
{
	int ret = 0;

	switch (tail_num)
	{
		case 0: case 5:
			ret = (today == MONDAY) ? 1 : 0; break;
		case 1: case 6:
			ret = (today == TUESDAY) ? 1 : 0; break;
		case 2: case 7:
			ret = (today == WEDNESDAY) ? 1 : 0; break;
		case 3: case 8:
			ret = (today == THURSDAY) ? 1 : 0; break;
		case 4: case 9:
			ret = (today == FRIDAY) ? 1 : 0; break;
		default:
			ret = 0; break;
	}
	return ret;
}

enum day get_week_day(int year, int month, int day)
{
	int origin_day = 2;
	int m[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int alldays = 0;
	int i;

	for (i = 1; i < month; i++)
		alldays += m[i - 1];

	alldays += day - 1;

	switch ((alldays + origin_day) % 7)
	{
		case 0: return SUNDAY;
		case 1: return MONDAY;
		case 2: return TUESDAY;
		case 3: return WEDNESDAY;
		case 4: return THURSDAY;
		case 5: return FRIDAY;
		case 6: return SATURDAY;
		default: return MONDAY;
	}
}

int main(void)
{
	char car_num[16];
	int year, month, day;
	int tail_num;
	enum day today;

	scanf("%s", car_num);
	scanf("%d %d %d", &year, &month, &day);

	tail_num = get_last_char(car_num) - '0';
	today = get_week_day(year, month, day);

	if (is_restricted(tail_num, today))
		printf("restricted!\n");
	else
		printf("NOT restricted!\n");

	return 0;
}
