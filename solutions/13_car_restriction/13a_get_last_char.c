#include <stdio.h>

char get_last_char(char str[]) {
    char c = 0;
    int i = 0;

    while (str[i]) {
        c = str[i];
        i++;
    }
    return c;
}

int main(void) {
    char buf[64];

    scanf("%s", buf);
    printf("%c\n", get_last_char(buf));

    return 0;
}
