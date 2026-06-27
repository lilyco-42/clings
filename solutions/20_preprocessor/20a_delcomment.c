#include <stdio.h>

int get_input_type(char c) {
    if (c == '/') return 1;
    if (c == '*') return 2;
    if (c == '\n') return 3;
    if (c == '\'') return 4;
    if (c == '\\') return 5;
    return 0;
}

int main(void) {
    int state = 0;

    while (1) {
        char c;
        int input;

        c = getchar();
        if (c == EOF) break;

        input = get_input_type(c);

        if (state == 0 && input == 1) {
            state = 1;
        } else if (state == 0 && input == 4) {
            state = 5;
            putchar(c);
        } else if (state == 1 && input == 1) {
            state = 4;
        } else if (state == 1 && input == 2) {
            state = 2;
        } else if (state == 1) {
            putchar('/');
            putchar(c);
            state = 0;
        } else if (state == 2 && input == 2) {
            state = 3;
        } else if (state == 2) {
            /* still in block comment */
        } else if (state == 3 && input == 1) {
            state = 0;
        } else if (state == 3 && input == 2) {
            state = 3;
        } else if (state == 3) {
            state = 2;
        } else if (state == 4 && input == 3) {
            state = 0;
            putchar(c);
        } else if (state == 4) {
            /* still in line comment */
        } else if (state == 5 && input == 4) {
            state = 0;
            putchar(c);
        } else if (state == 5 && input == 5) {
            state = 6;
            putchar(c);
        } else if (state == 6) {
            state = 5;
            putchar(c);
        } else if (state == 0 || state == 5) {
            putchar(c);
        }
    }

    return 0;
}
