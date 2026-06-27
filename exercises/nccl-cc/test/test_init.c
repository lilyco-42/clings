// expect: 0
/* test_init.c - Test array initializers and multi-variable declarations */
int main(void) {
    /* array initializer with explicit size */
    int a[3] = {10, 20, 30};
    if (a[0] != 10) return 1;
    if (a[1] != 20) return 2;
    if (a[2] != 30) return 3;

    /* multi-variable declaration */
    int x = 1, y = 2, z = 3;
    if (x != 1) return 4;
    if (y != 2) return 5;
    if (z != 3) return 6;

    /* sum of array */
    int sum = 0;
    for (int i = 0; i < 3; i = i + 1) sum = sum + a[i];
    if (sum != 60) return 7;

    return 0;
}
