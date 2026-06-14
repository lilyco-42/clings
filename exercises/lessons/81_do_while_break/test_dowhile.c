// expect: 120
int main(void) {
    int n = 1;
    int result = 1;
    do {
        result = result * n;
        n = n + 1;
    } while (n <= 5);
    return result;
}
