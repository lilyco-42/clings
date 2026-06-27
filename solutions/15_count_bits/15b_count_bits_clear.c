#include <stdio.h>

int count_bits(int num) {
    int sum = 0;

    while (num != 0) {
        num = num & (num - 1);
        sum++;
    }
    return sum;
}

int main(void) {
    int num;

    scanf("%d", &num);
    printf("%d\n", count_bits(num));

    return 0;
}
