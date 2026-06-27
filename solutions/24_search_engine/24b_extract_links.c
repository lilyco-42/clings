#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define MAX_URL 1024

static const char *find_istr(const char *text, const char *pattern) {
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

static int extract_links(const char *html) {
    const char *p = html;
    int count = 0;

    while ((p = find_istr(p, "href=")) != NULL) {
        p += 5;
        while (*p == ' ') p++;

        char quote = 0;
        if (*p == '"' || *p == '\'') {
            quote = *p++;
        }

        char url[MAX_URL];
        int i = 0;
        while (*p && i < MAX_URL - 1) {
            if (quote && *p == quote) break;
            if (!quote && (*p == ' ' || *p == '>' || *p == '\n')) break;
            url[i++] = *p++;
        }
        url[i] = '\0';

        if (i > 0) {
            printf("[%d] %s\n", ++count, url);
        }
    }
    return count;
}

int main(void) {
    char html[4096];
    int total = 0;

    int len = 0;
    int n;
    while ((n = fread(html + len, 1, sizeof(html) - len - 1, stdin)) > 0) len += n;
    html[len] = '\0';

    total = extract_links(html);
    printf("\nTotal: %d links found.\n", total);

    return 0;
}
