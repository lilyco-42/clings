/*
 * Lesson 25c: mycp 完整文件拷贝 — 参考答案
 */
#include <stdio.h>

#define BUF_SIZE 512

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s source_file dest_file\n", argv[0]);
        return 1;
    }

    FILE *fp1 = fopen(argv[1], "rb");
    if (fp1 == NULL) {
        perror(argv[1]);
        return 1;
    }

    FILE *fp2 = fopen(argv[2], "wb");
    if (fp2 == NULL) {
        perror(argv[2]);
        fclose(fp1);
        return 1;
    }

    char buf[BUF_SIZE];
    size_t n;

    while ((n = fread(buf, 1, sizeof(buf), fp1)) > 0) {
        if (fwrite(buf, 1, n, fp2) != n) {
            perror(argv[2]);
            fclose(fp1);
            fclose(fp2);
            return 1;
        }
    }

    if (ferror(fp1)) {
        perror(argv[1]);
        fclose(fp1);
        fclose(fp2);
        return 1;
    }

    fclose(fp1);
    fclose(fp2);
    return 0;
}
