/* invalid: '@' is not a valid C token - the tokenizer must reject it */
int main(void) { return 4 @2; }
