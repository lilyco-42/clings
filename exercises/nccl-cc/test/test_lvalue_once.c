// expect: 150
/* test_lvalue_once.c - lvalues with SIDE EFFECTS must be evaluated
 * exactly once by compound assignment and ++/--.
 * If the compiler naively rewrites `A op= B` as `A = A op B`, next()
 * runs twice per statement, idx drifts, and the result is garbage. */
int idx;
int a[5];

int next(void) {
    idx = idx + 1;
    return idx;
}

int main(void) {
    a[0] = 10; a[1] = 20; a[2] = 30; a[3] = 40; a[4] = 50;
    idx = -1;

    a[next()] += 5;     /* next() -> 0 once: a[0] = 15 */
    a[next()]++;        /* next() -> 1 once: a[1] = 21 */
    ++a[next()];        /* next() -> 2 once: a[2] = 31 */
    a[next()] <<= 1;    /* next() -> 3 once: a[3] = 80 */

    /* 15 + 21 + 31 + 80 + 3 = 150 */
    return a[0] + a[1] + a[2] + a[3] + idx;
}
