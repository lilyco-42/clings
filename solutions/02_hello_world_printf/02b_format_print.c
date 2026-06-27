#include <stdio.h>

int global = 200;

int main(void) {
    int local = 100;

    printf("local = %d\n", local);
    printf("global = 0x%x\n", global);

    return 0;
}
