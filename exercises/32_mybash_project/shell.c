#include <stdio.h>
#include <string.h>

int mycp_main(int, char **);
int readelf_main(int, char **);
int sed_main(int, char **);
int math_main(int, char **);
int more_main(int, char **);
int sort_main(int, char **);
int ll_main(int, char **);

struct operation {
	char name[16];
	int (*pf)(int, char **);
};

static struct operation op[] = {
	{ "mycp",    mycp_main },
	{ "readelf", readelf_main },
	{ "sed",     sed_main },
	{ "math",    math_main },
	{ "more",    more_main },
	{ "sort",    sort_main },
	{ "ll",      ll_main },
};

int command_do(int argc, char *argv[])
{
	if (argc < 1 || argv[0] == NULL)
		return -1;

	for (int i = 0; i < (int)(sizeof(op) / sizeof(op[0])); i++) {
		if (strcmp(argv[0], op[i].name) == 0)
			return op[i].pf(argc, argv);
	}

	fprintf(stderr, "command not found: %s\n", argv[0]);
	return -1;
}

#define MAX_ARGS 32

int shell_parse(char *buf)
{
	int argc = 0;
	char *argv[MAX_ARGS];
	int in_word = 0;

	while (*buf) {
		if (*buf == '\n') {
			*buf = '\0';
			break;
		}
		if (*buf == ' ' || *buf == '\t') {
			if (in_word) {
				*buf = '\0';
				in_word = 0;
			}
		} else {
			if (!in_word && argc < MAX_ARGS - 1) {
				argv[argc++] = buf;
				in_word = 1;
			}
		}
		buf++;
	}
	argv[argc] = NULL;

	if (argc == 0)
		return 0;

	return command_do(argc, argv);
}


