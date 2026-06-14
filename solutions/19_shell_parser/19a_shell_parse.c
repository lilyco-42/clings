#include <stdio.h>

int main(void)
{
	char buf[256];
	char *argv[32];
	int argc = 0;
	int state = 0;
	int i;

	fgets(buf, sizeof(buf), stdin);

	i = 0;
	while (buf[i] && buf[i] != '\n')
	{
		char c = buf[i];

		if (c == ' ' && state == 0)
			state = 0;
		else if (c != ' ' && state == 0)
		{
			argv[argc++] = &buf[i];
			state = 1;
		}
		else if (c == ' ' && state == 1)
		{
			buf[i] = '\0';
			state = 0;
		}

		i++;
	}
	if (buf[i] == '\n') buf[i] = '\0';

	for (i = 0; i < argc; i++)
	{
		if (i > 0) printf("|");
		printf("%s", argv[i]);
	}
	printf("\n");

	return 0;
}
