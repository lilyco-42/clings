/*
 * Lesson 26a: 读取 ELF 魔数 — 参考答案
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
    size_t n = fread(ident, 1, EI_NIDENT, fp);
    fclose(fp);

    if (n < EI_NIDENT) {
        fprintf(stderr, "%s: file too small\n", argv[1]);
        return 1;
    }

    unsigned char magic[] = { 0x7f, 'E', 'L', 'F' };
    if (memcmp(ident, magic, 4) != 0) {
        printf("Not an ELF file\n");
        return 1;
    }

    printf("ELF Magic: ");
    for (int i = 0; i < EI_NIDENT; i++)
        printf("%02x ", ident[i]);
    printf("\n");

    return 0;
}
