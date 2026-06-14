#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_URL 1024

static const char *find_istr(const char *text, const char *pattern)
{
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

static int extract_links(const char *html)
{
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

static char *read_all(FILE *fp)
{
	size_t cap = 4096, len = 0;
	char *buf = malloc(cap);
	if (!buf) return NULL;

	size_t n;
	while ((n = fread(buf + len, 1, cap - len - 1, fp)) > 0) {
		len += n;
		if (len + 1 >= cap) {
			cap *= 2;
			buf = realloc(buf, cap);
			if (!buf) return NULL;
		}
	}
	buf[len] = '\0';
	return buf;
}

int main(void)
{
	char *html = read_all(stdin);
	if (!html) { fprintf(stderr, "Failed to read input\n"); return 1; }

	int count = extract_links(html);
	printf("\nTotal: %d links found.\n", count);

	free(html);
	return 0;
}
