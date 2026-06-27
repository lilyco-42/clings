#include <ctype.h>
#include <stdio.h>
#include <string.h>

const char *find_istr(const char *text, const char *pattern) {
    int plen = strlen(pattern);
    while (*text) {
        int match = 1;
        for (int i = 0; i < plen && text[i]; i++) {
            if (tolower(text[i]) != tolower(pattern[i])) {
                match = 0;
                break;
            }
        }
        if (match) return text;
        text++;
    }
    return NULL;
}

int main(void) {
    char text[1024];
    char pattern[256];

    fgets(text, sizeof(text), stdin);
    text[strcspn(text, "\n")] = '\0';

    fgets(pattern, sizeof(pattern), stdin);
    pattern[strcspn(pattern, "\n")] = '\0';

    if (find_istr(text, pattern))
        printf("found\n");
    else
        printf("not found\n");

    return 0;
}
