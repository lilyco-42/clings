#include <stdio.h>

void itoa(int num, char *buf, int base) {
    char *hex = "0123456789ABCDEF";
    int i = 0;
    int j;

    do {
        int rest = num % base;
        buf[i++] = hex[rest];
        num /= base;
    } while (num != 0);

    buf[i] = '\0';

    for (j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }
}

int main(void) {
    int num, base;
    char buf[64];

    scanf("%d %d", &num, &base);

    itoa(num, buf, base);
    printf("%s\n", buf);

    return 0;
}
