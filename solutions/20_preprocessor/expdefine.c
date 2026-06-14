#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct macro
{
	char name[64];
	char value[64];
} macros[16];

int macro_count = 0;

void replace_and_print(char *line)
{
	char out[4096];
	int oi = 0;

	for (int i = 0; line[i]; )
	{
		int replaced = 0;
		for (int m = 0; m < macro_count; m++)
		{
			int nlen = strlen(macros[m].name);
			if (strncmp(&line[i], macros[m].name, nlen) == 0 &&
			    (i == 0 || !isalnum((unsigned char)line[i-1])) &&
			    !isalnum((unsigned char)line[i + nlen]))
			{
				for (int j = 0; macros[m].value[j]; j++)
					out[oi++] = macros[m].value[j];
				i += nlen;
				replaced = 1;
				break;
			}
		}
		if (!replaced)
			out[oi++] = line[i++];
	}
	out[oi] = '\0';
	printf("%s", out);
}

int main(void)
{
	char line[4096];

	while (fgets(line, sizeof(line), stdin))
	{
		if (strncmp(line, "#define ", 8) == 0)
		{
			char *p = line + 8;
			while (*p == ' ') p++;
			int i = 0;
			while (*p && !isspace((unsigned char)*p))
				macros[macro_count].name[i++] = *p++;
			macros[macro_count].name[i] = '\0';
			while (*p == ' ' || *p == '\t') p++;
			i = 0;
			while (*p && *p != '\n')
				macros[macro_count].value[i++] = *p++;
			macros[macro_count].value[i] = '\0';
			macro_count++;
		}
		else
		{
			replace_and_print(line);
		}
	}

	return 0;
}
