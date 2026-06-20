/*
 * mycp.c - 参考实现：文件拷贝
 *
 * 本文件为 Lesson 25 的参考代码，供学生阅读学习。
 * 学生练习文件为 25a/25b/25c。
 */
#include <stdio.h>

#define BUF_SIZE 512

int mycp_main(int argc, char *argv[])
{
    FILE *fp1, *fp2;
    char buf[BUF_SIZE];
    size_t n;

    if (argc < 3) {
        fprintf(stderr, "Usage: %s source_file dest_file\n", argv[0]);
        return 1;
    }

    fp1 = fopen(argv[1], "rb");
    if (fp1 == NULL) {
        perror(argv[1]);
        return 1;
    }

    fp2 = fopen(argv[2], "wb");
    if (fp2 == NULL) {
        perror(argv[2]);
        fclose(fp1);
        return 1;
    }

    while ((n = fread(buf, 1, BUF_SIZE, fp1)) > 0) {
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
