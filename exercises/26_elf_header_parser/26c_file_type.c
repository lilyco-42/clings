/*
 * Lesson 26c: ELF 文件类型识别
 *
 * 知识点：结构体数组查表, 字符串映射, 完整的 readelf 输出
 *
 * 任务：
 *   实现简化版 readelf，读取 ELF Header 并输出以下信息：
 *
 *   ELF Header:
 *     Magic:   7f 45 4c 46 ...（16 字节十六进制）
 *     Class:                             ELF32
 *     Type:                              REL (Relocatable file)
 *     Machine:                           0x3
 *     Entry point address:               0x0
 *     Start of section headers:          N (bytes into file)
 *     Size of this header:               52 (bytes)
 *     Number of section headers:         N
 *
 *   要求：
 *   1. e_type 映射为字符串:
 *      0 → "NONE (No file type)"
 *      1 → "REL (Relocatable file)"
 *      2 → "EXEC (Executable file)"
 *      3 → "DYN (Shared object file)"
 *      4 → "CORE (Core file)"
 *      其他 → "Unknown"
 *   2. 使用结构体数组 + 查表函数实现映射
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

#error TODO: Fix this exercise. Run "clings hint" for help.
/* 实现:
 * 1. type_string 结构体数组（type → 描述字符串的映射）
 * 2. get_type_string(int type) 查表函数
 * 3. main: 打开文件 → 读取 Header → 校验魔数 → 格式化输出
 */

int main(int argc, char *argv[])
{
    (void)argc; (void)argv;
    return 0;
}
