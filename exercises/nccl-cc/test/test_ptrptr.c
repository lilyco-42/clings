// expect: 42
/* test_ptrptr.c - pointer-to-pointer: the LP64 acid test.
 * On x86-64 a pointer is 8 bytes; any 4-byte truncation segfaults here. */
int main(void) {
    int x = 40;
    int *p = &x;
    int **pp = &p;
    **pp = **pp + 2;
    return x;
}
