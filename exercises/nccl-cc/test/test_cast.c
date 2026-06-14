// expect: 43
/* test_cast.c - (char) casts sign-extend the low byte */
int main(void) {
    int big = 300;
    char c = (char)big;    /* 300 & 0xff = 44 */
    int d = (char)511;     /* 0xff -> -1 (sign-extended) */
    return c + d;          /* 44 - 1 = 43 */
}
