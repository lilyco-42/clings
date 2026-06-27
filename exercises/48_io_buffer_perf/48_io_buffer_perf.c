/* 48_io_buffer_perf.c — 标准I/O缓冲性能对比：文件拷贝
 *
 * 任务: 实现两种文件拷贝方式并对比耗时
 *       1. copy_fgetc — fgetc/fputc 逐字节拷贝，每次都触发系统调用
 *       2. copy_fread — fread/fwrite 4KB 批量拷贝，减少系统调用次数
 *
 * 三种缓冲模式:
 *   全缓冲（_IOFBF）: 默认 FILE* 模式，缓冲区满了才刷盘
 *   行缓冲（_IOLBF）: stdout 连终端时，遇到 \n 刷新
 *   无缓冲（_IONBF）: stderr 立即输出
 *
 * 知识点: 全缓冲/行缓冲/无缓冲、系统调用开销、clock() 计时、
 *         CLOCKS_PER_SEC（每秒时钟数）
 *
 * 验证：生成 10MB 临时文件，分别用两种方式拷贝，输出耗时对比
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FILE_SIZE (10 * 1024 * 1024) /* 10MB */
#define BUF_SIZE 4096

void create_test_file(const char *path, int size) {
    FILE *f = fopen(path, "wb");
    for (int i = 0; i < size; i++) fputc('A' + (i % 26), f);
    fclose(f);
}

/* 逐字节拷贝：每次 fgetc 读 1 字节 + fputc 写 1 字节
 * 10MB → 约 1000 万次 fgetc + 1000 万次 fputc = 2000 万次 syscall！
 * 返回耗时（秒） */
double copy_fgetc(const char *src_path, const char *dst_path) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* clock_t start = clock() */

    /* fopen(src, "rb") + fopen(dst, "wb")，检查失败 */

    /* int ch; while ((ch = fgetc(fin)) != EOF) fputc(ch, fout) */

    /* fclose(fin); fclose(fout) */

    /* return (double)(clock() - start) / CLOCKS_PER_SEC */
}

/* 缓冲拷贝：每次 fread 读 4KB + fwrite 写 4KB
 * 10MB / 4KB ≈ 2500 次 fread/fwrite → 约 5000 次 syscall
 * 理论加速比 ~4000×，实际受磁盘速度限制通常在 5-100×
 * 返回耗时（秒） */
double copy_fread(const char *src_path, const char *dst_path) {
#error TODO: Finish this exercise. Run "clings hint" for help.
    /* clock_t start = clock() */

    /* fopen + 检查失败 + char buf[BUF_SIZE] */

    /* size_t n; while ((n = fread(buf, 1, BUF_SIZE, fin)) > 0) fwrite(buf, 1, n, fout) */

    /* fclose(fin); fclose(fout) */

    /* return (double)(clock() - start) / CLOCKS_PER_SEC */
}

int main(void) {
    const char *src = "/tmp/io_perf_src.tmp";
    const char *dst1 = "/tmp/io_perf_fgetc.tmp";
    const char *dst2 = "/tmp/io_perf_fread.tmp";

    create_test_file(src, FILE_SIZE);

    double t1 = copy_fgetc(src, dst1);
    double t2 = copy_fread(src, dst2);

    printf("file size: %d bytes\n", FILE_SIZE);
    printf("fgetc:   %.4f s\n", t1);
    printf("fread:   %.4f s (buf=%d)\n", t2, BUF_SIZE);
    if (t2 > 0) printf("speedup: %.1f×\n", t1 / t2);

    remove(src);
    remove(dst1);
    remove(dst2);
    return 0;
}
