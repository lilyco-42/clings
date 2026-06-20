/*
 * Lesson 26a: 读取 ELF 魔数
 *
 * 知识点：fread 读取固定字节, 十六进制打印, 魔数校验
 *
 * 任务：
 *   1. 以 "rb" 模式打开 argv[1] 指定的文件
 *   2. 读取前 16 字节（ELF e_ident 区域）
 *   3. 检查前 4 字节是否为 ELF 魔数: 0x7f 'E' 'L' 'F'
 *   4. 如果是 ELF，打印 "ELF Magic: " 后跟 16 字节十六进制（空格分隔），换行
 *   5. 如果不是 ELF，打印 "Not an ELF file\n" 并返回 1
 *
 * 输出格式示例：
 *   ELF Magic: 7f 45 4c 46 01 01 01 00 00 00 00 00 00 00 00 00
 */

#include <stdio.h>
#include <string.h>

#define EI_NIDENT 16

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s elf_file\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "rb");
    if (!fp) {
        perror(argv[1]);
        return 1;
    }

    unsigned char ident[EI_NIDENT];

#error TODO: Fix this exercise. Run "clings hint" for help.
    /* 1. fread 读取 EI_NIDENT 字节到 ident[]
     * 2. 检查 ident[0..3] 是否为 { 0x7f, 'E', 'L', 'F' }
     * 3. 是则打印 "ELF Magic: xx xx xx ...\n"
     * 4. 否则打印 "Not an ELF file\n" 并返回 1 */

    fclose(fp);
    return 0;
}
