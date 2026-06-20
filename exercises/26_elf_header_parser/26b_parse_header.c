/*
 * Lesson 26b: 解析 ELF Header 结构体
 *
 * 知识点：结构体定义, packed 属性, 类型转换, 无符号整数
 *
 * 任务：
 *   1. 定义 Elf32_Ehdr 结构体（使用正确的无符号类型 + packed 属性）
 *   2. 从文件读取整个 ELF Header（sizeof(Elf32_Ehdr) 字节）
 *   3. 校验 ELF 魔数
 *   4. 打印以下字段：
 *      Type:    %u
 *      Machine: %u
 *      Entry:   0x%x
 *
 * 注意: ELF 类型必须使用无符号整数！
 *   - Elf32_Half = unsigned short (2 字节)
 *   - Elf32_Word = unsigned int   (4 字节)
 *   - Elf32_Addr = unsigned int   (4 字节)
 *   - Elf32_Off  = unsigned int   (4 字节)
 */

#include <stdio.h>
#include <string.h>

#define EI_NIDENT 16

/* TODO: 定义正确的 ELF 类型和结构体 */
#error TODO: Fix this exercise. Run "clings hint" for help.

/*
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
*/

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s elf_file\n", argv[0]);
        return 1;
    }

    /* 打开文件, 读取 header, 校验魔数, 打印字段 */
    return 0;
}
