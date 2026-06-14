// expect: 9
/* test_enum.c - enum constants fold at compile time; top-level and
 * function-scope declarations; usable as case labels */
enum Color { RED, GREEN = 5, BLUE };

int main(void) {
    enum Op { NEG = -2, ZERO = 0, ONE };
    int s = 0;
    switch (GREEN) {
    case GREEN:
        s = BLUE;          /* 6 */
        break;
    default:
        s = RED;
    }
    /* 6 + 1 + (-2) + 2 + 0 + 2 = 9 */
    return s + ONE + NEG + 2 + RED + 2 * (BLUE == 6);
}
