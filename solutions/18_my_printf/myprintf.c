#include <stdarg.h>
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

void myputs(char *buf) {
    while (*buf) putchar(*buf++);
}

int myprintf(const char *format, ...) {
    va_list ap;
    char c;
    char buf[32];

    va_start(ap, format);

    while ((c = *format++) != '\0') {
        if (c != '%') {
            putchar(c);
            continue;
        }

        c = *format++;
        if (c == 's')
            myputs(va_arg(ap, char *));
        else if (c == 'c')
            putchar(va_arg(ap, int));
        else if (c == 'd') {
            itoa(va_arg(ap, int), buf, 10);
            myputs(buf);
        } else if (c == 'x') {
            itoa(va_arg(ap, int), buf, 16);
            myputs(buf);
        }
    }

    va_end(ap);
    return 0;
}

int main(void) {
    myprintf("a = %d, b = 0x%x\n", 100, 200);
    myprintf("c = %c, s = %s\n", 'A', "helloworld");

    return 0;
}
