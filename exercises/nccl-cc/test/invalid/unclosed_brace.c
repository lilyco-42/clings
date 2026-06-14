/* invalid: unclosed brace - the parser must terminate with a
 * diagnostic instead of looping forever at end of file */
int main(void) {
    if (1) {
        return 1;
