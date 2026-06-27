// expect: 25
/* test_ternary.c - ternary, comma operator, short-circuit logic */
int main(void) {
    int a = 5;
    int b = (a > 3) ? 20 : 10;
    int c = (1, 2, 3); /* comma: value is 3 */
    int d = (a == 5 && b == 20) ? 1 : 0;
    int e = (0 || d) ? 1 : 99;
    return b + c + d + e; /* 20 + 3 + 1 + 1 = 25 */
}
