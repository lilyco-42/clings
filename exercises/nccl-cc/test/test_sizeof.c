// expect: 21
/* test_sizeof.c - sizeof is fully type-driven. Written so the expected
 * value is the same on ILP32 (RV32/ARM) and LP64 (x86-64): pointer
 * sizes differ across targets, so we only compare them to each other. */
int main(void) {
    int x;
    int a[6];
    int ok = 0;
    if (sizeof(int) == 4) ok = ok + 1;
    if (sizeof(char) == 1) ok = ok + 2;
    if (sizeof(x) == 4) ok = ok + 4;
    if (sizeof(a) == 24) ok = ok + 8;              /* arrays: full size */
    if (sizeof(int *) == sizeof(char *)) ok = ok + 6;
    return ok;   /* 1 + 2 + 4 + 8 + 6 = 21 */
}
