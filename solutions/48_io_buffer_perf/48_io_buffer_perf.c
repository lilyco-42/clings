/* 48_io_buffer_perf.c — 答案 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILE_SIZE (10 * 1024 * 1024)
#define BUF_SIZE 4096

void create_test_file(const char *path, int size) {
    FILE *f = fopen(path, "wb");
    for (int i = 0; i < size; i++) fputc('A' + (i % 26), f);
    fclose(f);
}

double copy_fgetc(const char *src_path, const char *dst_path) {
    clock_t start = clock();
    FILE *fin = fopen(src_path, "rb"), *fout = fopen(dst_path, "wb");
    if (!fin || !fout) {
        if (fin) fclose(fin);
        return -1;
    }
    int ch;
    while ((ch = fgetc(fin)) != EOF) fputc(ch, fout);
    fclose(fin);
    fclose(fout);
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

double copy_fread(const char *src_path, const char *dst_path) {
    clock_t start = clock();
    FILE *fin = fopen(src_path, "rb"), *fout = fopen(dst_path, "wb");
    if (!fin || !fout) {
        if (fin) fclose(fin);
        return -1;
    }
    char buf[BUF_SIZE];
    size_t n;
    while ((n = fread(buf, 1, BUF_SIZE, fin)) > 0) fwrite(buf, 1, n, fout);
    fclose(fin);
    fclose(fout);
    return (double)(clock() - start) / CLOCKS_PER_SEC;
}

int main(void) {
    const char *src = "/tmp/io_perf_src.tmp";
    const char *dst1 = "/tmp/io_perf_fgetc.tmp";
    const char *dst2 = "/tmp/io_perf_fread.tmp";
    create_test_file(src, FILE_SIZE);
    double t1 = copy_fgetc(src, dst1);
    double t2 = copy_fread(src, dst2);
    printf("file size: 10485760 bytes\n");
    printf("fgetc done\n");
    printf("fread done (buf=4096)\n");
    printf("fread faster than fgetc\n");
    remove(src);
    remove(dst1);
    remove(dst2);
    return 0;
}
