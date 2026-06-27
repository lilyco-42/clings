#include <stdio.h>

enum day { MONDAY = 1, TUESDAY, WEDNESDAY, THURSDAY, FRIDAY, SATURDAY, SUNDAY };

int is_restricted(int tail_num, enum day today) {
    int ret = 0;

    switch (tail_num) {
        case 0:
        case 5:
            ret = (today == MONDAY) ? 1 : 0;
            break;
        case 1:
        case 6:
            ret = (today == TUESDAY) ? 1 : 0;
            break;
        case 2:
        case 7:
            ret = (today == WEDNESDAY) ? 1 : 0;
            break;
        case 3:
        case 8:
            ret = (today == THURSDAY) ? 1 : 0;
            break;
        case 4:
        case 9:
            ret = (today == FRIDAY) ? 1 : 0;
            break;
        default:
            ret = 0;
            break;
    }
    return ret;
}

int main(void) {
    int weekday, tail_num;

    scanf("%d %d", &weekday, &tail_num);

    if (is_restricted(tail_num, (enum day)weekday))
        printf("restricted\n");
    else
        printf("free\n");

    return 0;
}
