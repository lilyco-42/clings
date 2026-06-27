// expect: 0
/* test_bitwise.c - Test bitwise operators & | ^ ~ << >> */
int main(void) {
    int a = 0xff;
    int b = 0x0f;

    /* bitwise AND */
    int and_result = a & b; /* 0x0f = 15 */
    if (and_result != 15) return 1;

    /* bitwise OR */
    int or_result = a | 0x100; /* 0x1ff = 511 */
    if (or_result != 511) return 2;

    /* bitwise XOR */
    int xor_result = a ^ b; /* 0xf0 = 240 */
    if (xor_result != 240) return 3;

    /* bitwise NOT */
    int not_result = ~0; /* -1 */
    if (not_result != -1) return 4;

    /* left shift */
    int shl = 1 << 4; /* 16 */
    if (shl != 16) return 5;

    /* right shift */
    int shr = 256 >> 3; /* 32 */
    if (shr != 32) return 6;

    /* compound assignment */
    int x = 0xff;
    x &= 0x0f;
    if (x != 15) return 7;

    x |= 0xf0;
    if (x != 255) return 8;

    x ^= 0xff;
    if (x != 0) return 9;

    return 0;
}
