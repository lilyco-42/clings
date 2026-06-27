#include <math.h>
#include <stdio.h>

int main(void) {
    int num;
    int i;
    int max = 0;

    for (num = 1; num <= 100; num++) {
        int tmp = (int)sqrt((double)num);

        for (i = 2; i <= tmp; i++) {
            if (num % i == 0) break;
        }

        if (i == tmp + 1) max = num;
    }

    printf("max prime is %d\n", max);

    return 0;
}
