#include <stdio.h>

#define ALL 10
#define OUT 3

int next[ALL];

void init_ring(void) {
    int i;
    for (i = 0; i < ALL; i++) next[i] = (i + 1) % ALL;
}

int main(void) {
    int left;
    int counter;
    int i;
    int prev;

    init_ring();

    left = ALL;
    counter = 0;
    i = 0;
    prev = ALL - 1;

    while (left > 0) {
        counter++;

        if (counter == OUT) {
            left--;
            printf("%d is out\n", i + 1);
            next[prev] = next[i];
            counter = 0;
        }

        prev = i;
        i = next[i];
    }

    return 0;
}
