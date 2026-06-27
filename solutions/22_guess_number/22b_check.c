#include <stdio.h>

void check(const int secret[4], const int guess[4], int *a, int *b) {
    *a = *b = 0;
    for (int i = 0; i < 4; i++) {
        if (guess[i] == secret[i]) {
            (*a)++;
        } else {
            for (int j = 0; j < 4; j++) {
                if (guess[i] == secret[j]) {
                    (*b)++;
                    break;
                }
            }
        }
    }
}

int main(void) {
    int secret[4] = {1, 2, 3, 4};
    int guess[4];
    int a, b;
    int i;
    char input[16];

    fgets(input, sizeof(input), stdin);
    for (i = 0; i < 4; i++) guess[i] = input[i] - '0';

    check(secret, guess, &a, &b);
    printf("%dA%dB\n", a, b);

    return 0;
}
