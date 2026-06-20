/*
 * Lesson 29c: 完整 more 分页浏览器 — 参考答案 (Linux-only)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MAX_LINES 65536

static struct termios orig_ts;
static char *pv[MAX_LINES];
static int lines = 0;

static void get_term_size(int *w, int *h)
{
    struct winsize ws;
    memset(&ws, 0, sizeof(ws));
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ws);
    *w = ws.ws_col;
    *h = ws.ws_row;
}

static void set_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &orig_ts);
    struct termios ts = orig_ts;
    ts.c_lflag &= ~(ECHO | ICANON);
    ts.c_cc[VMIN] = 1;
    ts.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ts);
}

static void restore_term(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_ts);
}

static void free_lines(void)
{
    for (int i = 0; i < lines; i++)
        free(pv[i]);
    lines = 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror(argv[1]);
        return 1;
    }

    char buf[1024];
    while (fgets(buf, sizeof(buf), fp) && lines < MAX_LINES) {
        pv[lines] = malloc(strlen(buf) + 1);
        if (pv[lines] == NULL)
            break;
        strcpy(pv[lines], buf);
        lines++;
    }
    fclose(fp);

    int width, height;
    get_term_size(&width, &height);
    (void)width;

    set_raw_mode();

    int top = 0;
    int dropdown;

    if (lines <= height - 1) {
        dropdown = lines;
        goto quit;
    }
    dropdown = height - 1;

    while (1) {
        system("clear");
        for (int i = 0; i < dropdown; i++)
            printf("%s", pv[top + i]);
        printf(":");

        int c = getc(stdin);
        if (c == EOF)
            break;

        switch (c) {
        case 'q':
            goto quit;
        case 'k':
            if (top > 0) top -= 1;
            break;
        case '\n':
        case 'j':
            if (top + 1 + dropdown <= lines) top += 1;
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
            if (top < 0) top = 0;
            break;
        }
    }

quit:
    system("clear");
    for (int i = 0; i < dropdown; i++)
        printf("%s", pv[top + i]);
    restore_term();
    free_lines();
    return 0;
}
