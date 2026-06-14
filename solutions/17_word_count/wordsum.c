#include <stdio.h>

int get_input_type(char c)
{
	if (c >= 'a' && c <= 'z')
		return 1;

	if (c >= 'A' && c <= 'Z')
		return 1;

	return 0;
}

int main(void)
{
	char buf[512];
	int state = 0;
	int i = 0;
	int words = 0;
	char *p = NULL;
	int counter = 0;

	fgets(buf, sizeof(buf), stdin);
	for (i = 0; buf[i]; i++)
		if (buf[i] == '\n') { buf[i] = '\0'; break; }

	i = 0;
	while (1)
	{
		char c;
		int input = 0;

		c = buf[i];
		input = get_input_type(c);

		if (c == '\0')
			break;

		if (state == 0 && input == 0)
		{
			state = 0;
		}
		else
		if (state == 0 && input == 1)
		{
			state = 1;
			p = &buf[i];
			counter = 0;
			counter++;
		}
		else
		if (state == 1 && input == 0)
		{
			int j;
			state = 0;
			words++;
			printf("word %d found!\n", words);
			for (j = 0; j < counter; j++)
			{
				printf("%c", p[j]);
			}
			printf("\n");
		}
		else
		if (state == 1 && input == 1)
		{
			state = 1;
			counter++;
		}

		i++;
	}

	printf("there is %d words found!\n", words);
	return 0;
}
