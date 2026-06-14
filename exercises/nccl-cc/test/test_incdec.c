// expect: 28
/* test_incdec.c - postfix yields the OLD value, prefix the new one;
 * pointer ++ advances by element size */
int main(void) {
    int i = 3;
    int j = i++;    /* j = 3, i = 4 */
    int k = ++i;    /* k = 5, i = 5 */

    int a[3];
    a[0] = 7; a[1] = 8; a[2] = 9;
    int *p = a;
    int v = *p++;   /* v = a[0] = 7, p -> a[1] */
    int w = *p;     /* 8 */

    i--;            /* 4 */

    return j + k + v + w + i + (p - a);   /* 3+5+7+8+4+1 = 28 */
}
