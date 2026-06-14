// expect: 20
int main(void) {
    int x = 10;
    int *p = &x;
    *p = 20;
    return x;
}
