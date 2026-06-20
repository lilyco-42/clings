/*
 * Lesson 25b: fread/fwrite 文件读写 — 参考答案
 */
#include <stdio.h>

#define BUF_SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s filename\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror(argv[1]);
        return 1;
    }

    char buf[BUF_SIZE];
    size_t n;

    while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
        fwrite(buf, 1, n, stdout);
    }

    if (ferror(fp)) {
        perror(argv[1]);
        fclose(fp);
        return 1;
    }

    fclose(fp);
    return 0;
}
