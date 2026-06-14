// expect: 35
/* test_struct.c - REAL struct test: member read/write, &, -> access */
struct Point {
    int x;
    int y;
};

int main(void) {
    struct Point p;
    struct Point *q;

    p.x = 10;
    p.y = 20;

    q = &p;
    q->y = q->y + 5;     /* p.y = 25 */

    return p.x + q->y;   /* 10 + 25 = 35 */
}
