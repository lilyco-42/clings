// expect: 30
int main(void) {
    int a[41];
    int n = 41;
    int m = 3;
    int i = 0;
    while (i < n) {
        a[i] = 1;
        i = i + 1;
    }

    int count = 0;
    i = -1;
    int remain = n;

    while (remain > 1) {
        i = i + 1;
        if (i >= n) i = 0;
        if (a[i] == 0) continue;
        count = count + 1;
        if (count == m) {
            a[i] = 0;
            remain = remain - 1;
            count = 0;
        }
    }

    i = 0;
    while (i < n) {
        if (a[i] == 1)
            return i;
        i = i + 1;
    }
    return -1;
}
