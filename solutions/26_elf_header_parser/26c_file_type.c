/*
 * Lesson 26c: ELF 文件类型识别 — 参考答案
 */
#include <stdio.h>
#include <string.h>

#define EI_NIDENT 16

typedef unsigned char  Elf32_Char;
typedef unsigned short Elf32_Half;
typedef unsigned int   Elf32_Word;
typedef unsigned int   Elf32_Addr;
typedef unsigned int   Elf32_Off;

typedef struct __attribute__((packed)) {
    Elf32_Char  e_ident[EI_NIDENT];
    Elf32_Half  e_type;
    Elf32_Half  e_machine;
    Elf32_Word  e_version;
    Elf32_Addr  e_entry;
    Elf32_Off   e_phoff;
    Elf32_Off   e_shoff;
    Elf32_Word  e_flags;
    Elf32_Half  e_ehsize;
    Elf32_Half  e_phentsize;
    Elf32_Half  e_phnum;
    Elf32_Half  e_shentsize;
    Elf32_Half  e_shnum;
    Elf32_Half  e_shstrndx;
} Elf32_Ehdr;

#define HSIZE ((int)sizeof(Elf32_Ehdr))

struct type_string {
    int type;
    const char *string;
} type_table[] = {
    { 0, "NONE (No file type)" },
    { 1, "REL (Relocatable file)" },
    { 2, "EXEC (Executable file)" },
    { 3, "DYN (Shared object file)" },
    { 4, "CORE (Core file)" },
    { -1, "Unknown" },
};

static const char *get_type_string(int type)
{
    int count = sizeof(type_table) / sizeof(type_table[0]);
    for (int i = 0; i < count - 1; i++)
        if (type_table[i].type == type)
            return type_table[i].string;
    return type_table[count - 1].string;
}

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

    char buf[HSIZE];
    size_t n = fread(buf, 1, HSIZE, fp);
    fclose(fp);

    if (n != (size_t)HSIZE) {
        fprintf(stderr, "%s: read %zu bytes, expected %d\n", argv[1], n, HSIZE);
        return 1;
    }

    Elf32_Ehdr *p = (Elf32_Ehdr *)buf;

    if (memcmp(p->e_ident, "\x7f""ELF", 4) != 0) {
        fprintf(stderr, "%s: not an ELF file\n", argv[1]);
        return 1;
    }

    printf("ELF Header:\n");
    printf("  Magic:   ");
    for (int i = 0; i < EI_NIDENT; i++)
        printf("%02x ", (unsigned char)p->e_ident[i]);
    printf("\n");
    printf("  Class:                             ELF32\n");
    printf("  Type:                              %s\n", get_type_string(p->e_type));
    printf("  Machine:                           0x%x\n", p->e_machine);
    printf("  Entry point address:               0x%x\n", p->e_entry);
    printf("  Start of section headers:          %u (bytes into file)\n", p->e_shoff);
    printf("  Size of this header:               %u (bytes)\n", p->e_ehsize);
    printf("  Number of section headers:         %u\n", p->e_shnum);

    return 0;
}
