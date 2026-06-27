#include <ctype.h>
#include <stdio.h>
#include <string.h>

int get_input_type(char *word) {
    if (strcmp(word, " ") == 0 || strcmp(word, "\t") == 0) return 0;
    if (strcmp(word, "#") == 0) return 1;
    if (strcmp(word, "define") == 0) return 2;
    if (strcmp(word, "\n") == 0) return 4;
    return 3;
}

char word[64];
char word_buf[64];
char buf[128];

struct macro {
    char name[64];
    char value[64];
} macros[16];

int macro_counter = 0;

void act_print_word(void) {
    int i;
    for (i = 0; i < macro_counter; i++) {
        if (strcmp(word, macros[i].name) == 0) {
            printf("%s", macros[i].value);
            return;
        }
    }
    printf("%s", word);
}

void act_save_to_buf(void) { strcat(buf, word); }

void act_print_buf_and_word(void) {
    printf("%s", buf);
    printf("%s", word);
    strcpy(buf, "");
}

void act_save_word(void) {
    strcat(word_buf, word);
    strcat(buf, word);
}

void act_get_macro_name(void) {
    strcpy(macros[macro_counter].name, word_buf);
    strcpy(word_buf, "");
}

void act_get_macro_value(void) {
    strcpy(macros[macro_counter].value, word_buf);
    strcpy(word_buf, "");
    strcpy(buf, "");
    strcpy(word, "");
    macro_counter++;
}

void act_null(void) {}

enum { s0 = 0, s1, s2, s3, s4, s5, s6, s7 };

int state_transition[8][5] = {
    {s0, s1, s7, s7, s0}, {s1, s0, s2, s0, s0}, {s3, s0, s0, s0, s0}, {s3, s4, s4, s4, s0},
    {s5, s5, s4, s0, s0}, {s5, s6, s6, s6, s0}, {s6, s6, s6, s6, s0}, {s7, s7, s7, s7, s0},
};

typedef void (*PF)(void);
PF act_table[8][5] = {
    {act_print_word, act_save_to_buf, act_print_word, act_print_word, act_print_word},
    {act_save_to_buf, act_print_buf_and_word, act_save_to_buf, act_print_buf_and_word, act_print_buf_and_word},
    {act_save_to_buf, act_print_buf_and_word, act_print_buf_and_word, act_print_buf_and_word, act_print_buf_and_word},
    {act_save_to_buf, act_save_word, act_save_word, act_save_word, act_print_buf_and_word},
    {act_get_macro_name, act_get_macro_name, act_save_word, act_print_buf_and_word, act_get_macro_value},
    {act_null, act_save_word, act_save_word, act_save_word, act_get_macro_value},
    {act_save_word, act_save_word, act_save_word, act_save_word, act_get_macro_value},
    {act_print_word, act_print_word, act_print_word, act_print_word, act_print_word},
};

void getword(char *word) {
    char c;

    c = getchar();

    if (c == EOF) {
        *word = '\0';
        return;
    }

    if (!isalpha(c)) {
        *word++ = c;
        *word = '\0';
        return;
    }

    do {
        *word++ = c;
        c = getchar();
    } while (isalnum(c) || c == '_');

    ungetc(c, stdin);
    *word = '\0';
}

int main(void) {
    int state = 0;

    while (1) {
        int input;
        void (*pf)(void);

        getword(word);
        if (strcmp(word, "") == 0) break;

        input = get_input_type(word);
        pf = act_table[state][input];
        pf();
        state = state_transition[state][input];
    }

    return 0;
}
