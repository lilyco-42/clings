union endian_test {
    unsigned char c[4];
    int i;
};

int main(void) {
    union endian_test u;

    u.i = 1;
    if (u.c[0] == 1)
        return 1;
    else
        return 0;
}
