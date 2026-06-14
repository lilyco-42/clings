// expect: 42
// Large stack frames break naive immediate encodings:
//   RISC-V: addi / lw / sw immediates are 12-bit signed (max 2047)
//   ARM:    sub #imm is an 8-bit rotated constant; ldr/str is +-4095
//   AArch64: sub #imm is 12-bit unsigned (max 4095)
// main's frame here is ~4800 bytes and fill's is ~2400, so every
// backend must take its "offset too big for the immediate" path for
// frame setup, local addressing, and declaration-init stores.
int fill(int n) {
    int buf[600];
    int i = 0;
    while (i < 600) {
        buf[i] = i;
        i = i + 1;
    }
    return buf[n];
}

int main(void) {
    int a[600];
    int b[600];
    int i = 0;
    while (i < 600) {
        a[i] = i / 2;
        b[i] = i / 3;
        i = i + 1;
    }
    return a[40] + b[60] + fill(2);
}
