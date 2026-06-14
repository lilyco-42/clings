// expect: 1
int main(void) {
    int a[5];
    a[0] = 5;
    a[1] = 3;
    a[2] = 8;
    a[3] = 1;
    a[4] = 4;

    int i = 0;
    while (i < 4) {
        int j = 0;
        while (j < 4 - i) {
            if (a[j] > a[j + 1]) {
                int tmp = a[j];
                a[j] = a[j + 1];
                a[j + 1] = tmp;
            }
            j = j + 1;
        }
        i = i + 1;
    }
    return a[0];
}
