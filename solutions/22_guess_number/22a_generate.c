#include <stdio.h>

/* 平台无关的伪随机数生成器 (Linear Congruential Generator) */
static unsigned int _seed = 42;
int my_rand(void) {
    _seed = _seed * 1103515245 + 12345;
    return (_seed >> 16) & 0x7fff;
}

void generate_secret(int secret[4]) {
    int used[10] = {0};
    int i;

    for (i = 0; i < 4; i++) {
        int d;
        do {
            d = my_rand() % 10;
        } while (used[d]);
        secret[i] = d;
        used[d] = 1;
    }
}

int main(void) {
    int secret[4];
    int i;

    generate_secret(secret);

    for (i = 0; i < 4; i++) printf("%d", secret[i]);
    printf("\n");

    return 0;
}
