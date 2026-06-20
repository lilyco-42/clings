/*
 * Lesson 25b: fread/fwrite 文件读写
 *
 * 知识点：fread, fwrite, 循环读取直到文件末尾, ferror
 *
 * 任务：
 *   1. 以 "rb" 模式打开 argv[1] 指定的文件
 *   2. 循环使用 fread 读取最多 512 字节
 *   3. 将读到的内容用 fwrite 写到 stdout
 *   4. 当 fread 返回 0 时停止循环
 *   5. 检查 ferror(fp) 判断是否读取出错
 *   6. 关闭文件
 *
 * 要点：不要用 feof() 做循环条件！正确做法：
 *       while ((n = fread(buf, 1, sizeof(buf), fp)) > 0)
 *
 * 验证：./25b_read_write test.txt 应输出 test.txt 的全部内容
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

#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 循环: while ((n = fread(...)) > 0) { fwrite(...) }
     * 循环结束后检查 ferror(fp) */

    fclose(fp);
    return 0;
}
