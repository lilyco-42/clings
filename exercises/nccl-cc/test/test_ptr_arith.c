// expect: 10
/* test_ptr_arith.c - pointer arithmetic: scaling, ptr-ptr, comparison */
int main(void) {
    int a[5];
    int *p;
    int i;
    for (i = 0; i < 5; i++)
        a[i] = i * 10;

    p = a;
    p = p + 2;            /* &a[2] */

    int d = p - a;        /* element count: 2 */
    int v = *(p + 1);     /* a[3] = 30 */

    return d + v / 10 + (p > a) + a[4] / 10;   /* 2 + 3 + 1 + 4 = 10 */
}
