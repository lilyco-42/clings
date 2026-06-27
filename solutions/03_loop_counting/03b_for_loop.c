#include <stdio.h>

int main(void) {
    int i;
    int sum = 0;

    for (i = 10; i >= 1; i--) {
        printf("counter = %d\n", i);
    }

    i = 1;
    do {
        sum += i;
        i++;
    } while (i <= 10);

    printf("sum = %d\n", sum);

    return 0;
}
