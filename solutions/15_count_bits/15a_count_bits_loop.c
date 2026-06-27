#include <stdio.h>

int count_bits(int num) {
    int sum = 0;
    int i;

    for (i = 0; i < 32; i++) {
        if (num & (1 << i)) sum++;
    }
    return sum;
}

int main(void) {
    int num;

    scanf("%d", &num);
    printf("%d\n", count_bits(num));

    return 0;
}
