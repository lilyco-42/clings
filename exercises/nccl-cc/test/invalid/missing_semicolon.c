/* invalid: missing semicolon - the compiler must exit non-zero
 * and the test runner asserts that it does */
int main(void) {
    return 42
}
