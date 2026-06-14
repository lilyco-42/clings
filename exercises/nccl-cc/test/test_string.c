// expect: 11
/* test_string.c - string literals: escapes decoded at lex time,
 * adjacent literals concatenated, printf %s via the runtime */
int main(void) {
    char *s = "hello\n";
    char *t = "ab" "cd";
    printf("%s", s);
    printf("t=%s len=%d\n", t, 4);
    return (s[5] == 10) + (t[0] == 'a') + (t[3] == 'd') + 8;   /* 1+1+1+8 */
}
