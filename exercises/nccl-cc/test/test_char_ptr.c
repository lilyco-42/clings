// expect: 98
/* test_char_ptr.c - byte-wide stores through char pointers */
int main(void) {
    char buf[4];
    char *s = buf;

    *s = 'a';
    s[1] = 'b';
    *(s + 2) = 0;

    return buf[0] - 'a' + buf[1];   /* 0 + 'b' = 98 */
}
