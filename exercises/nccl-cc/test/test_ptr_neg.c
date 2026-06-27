// expect: 60
/* test_ptr_neg.c - NEGATIVE pointer offsets.
 * On LP64 targets the scaled index is computed in 32 bits; without
 * sign extension before the 64-bit pointer add, -2 becomes a huge
 * positive offset and the load faults. */
int main(void) {
    int a[5];
    int i;
    for (i = 0; i < 5; i++) a[i] = (i + 1) * 10;

    int *p = a + 3;
    int d = -2;
    int *q = p + d; /* &a[1] */
    int neg = -1;

    return *q + a[4 + neg]; /* 20 + 40 = 60 */
}
