#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

static int width;
static int height;
static struct termios orig_ts;  // 保存原始终端属性，退出时恢复

static void get_termios(void)
{
	struct winsize ws;  // 栈变量，避免 malloc 泄漏
	memset(&ws, 0, sizeof(ws));
	ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
	width = ws.ws_col;
	height = ws.ws_row;
}

static void set_raw_mode(void)
{
	tcgetattr(STDIN_FILENO, &orig_ts);  // 保存原始属性
	struct termios ts = orig_ts;
	ts.c_lflag &= ~(ECHO | ICANON);    // 关闭回显和行缓冲
	ts.c_cc[VMIN] = 1;
	ts.c_cc[VTIME] = 0;
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts);
}

static void restore_term(void)
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_ts);  // 恢复原始属性
}

#define MAX_LINES 65536

static char *pv[MAX_LINES];  // 行指针数组
static int lines = 0;
static int top = 0;
static int dropdown = 0;

static void display(void)
{
	for (int i = 0; i < dropdown; i++)
		printf("%s", pv[top + i]);
}

static void free_lines(void)
{
	for (int i = 0; i < lines; i++)
		free(pv[i]);
	lines = 0;
}

int more_main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: more <filename>\n");
		return 1;
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL) {
		perror(argv[1]);
		return 1;
	}

	char buf[1024];
	lines = 0;
	while (fgets(buf, sizeof(buf), fp) && lines < MAX_LINES) {
		pv[lines] = malloc(strlen(buf) + 1);
		if (pv[lines] == NULL) {
			fprintf(stderr, "malloc failed\n");
			break;
		}
		strcpy(pv[lines], buf);
		lines++;
	}
	fclose(fp);

	get_termios();
	set_raw_mode();

	top = 0;
	if (lines <= height - 1) {
		dropdown = lines;
		goto quit;
	}
	dropdown = height - 1;

	while (1) {
		system("clear");
		display();
		printf(":");

		int c = getc(stdin);  // int 才能正确接收 EOF
		if (c == EOF)
			break;

		switch (c) {
		case 'q':
			goto quit;
		case 'k':
			if (top > 0)
				top -= 1;
			break;
		case '\n':
		case 'j':
			if (top + 1 + dropdown <= lines)
				top += 1;
			break;
		case ' ':
		case 'f':
			if (top + height - 1 + dropdown > lines) {
				top = lines - dropdown;
				goto quit;
			}
			top += height - 1;
			break;
		case 'b':
			top -= height - 1;
			if (top < 0)
				top = 0;
			break;
		}
	}

quit:
	system("clear");
	display();
	restore_term();  // 恢复终端到原始状态
	free_lines();
	return 0;
}
