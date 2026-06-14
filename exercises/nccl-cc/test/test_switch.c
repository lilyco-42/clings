// expect: 22
/* test_switch.c - switch with break, fallthrough-free bodies, default,
 * and switch inside a called function */
int classify(int v) {
    switch (v) {
    case 1: return 10;
    case 2: return 20;
    default: return 30;
    }
}

int main(void) {
    int s = 0;
    int x = 2;
    switch (x) {
    case 1:
        s = 1;
        break;
    case 2:
        s = 2;
        break;
    default:
        s = 9;
    }
    return classify(s) + s;   /* 20 + 2 = 22 */
}
