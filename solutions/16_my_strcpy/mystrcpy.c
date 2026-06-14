#include <stdio.h>
#include <assert.h>

char *mystrcpy(char *dest, const char *src)
{
	assert(dest != NULL && src != NULL);

	char *p = dest;

	while ((*dest++ = *src++) != '\0');

	return p;
}

int main(void)
{
	char s1[256] = "";
	char s2[256];

	fgets(s2, sizeof(s2), stdin);
	int i = 0;
	while (s2[i] && s2[i] != '\n') i++;
	s2[i] = '\0';

	mystrcpy(s1, s2);
	printf("%s\n", s1);

	return 0;
}
