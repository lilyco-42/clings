/* invalid: missing ')' in the if condition - skip() must report and
 * recover (never exit(1) mid-parse), and the compiler still fails */
int main(void) {
    int x = 1;
    if (x == 1 {
        return 2;
    }
    return 3;
}
