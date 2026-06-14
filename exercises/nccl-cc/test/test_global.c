// expect: 35
int g = 10;

int main(void) {
    int x = g + 5;
    g = 20;
    return g + x;
}
