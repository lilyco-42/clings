/*
 * bare_rv.ld - linker script for bare-metal RV32 (qemu -machine virt)
 *
 * The virt board's mask ROM unconditionally jumps to 0x80000000 (the
 * RAM base) when started with -bios none, so _start must be the very
 * first byte there. Two things guarantee that:
 *   1. runtime_rv_bare.s puts _start in .text.start, placed first;
 *   2. .note sections are discarded (ld's build-id note is otherwise
 *      ALLOC and silently steals the 0x80000000 slot - the entry
 *      point shifts and the board executes note bytes as code).
 * Link with: -Wl,--build-id=none -T bare_rv.ld
 */
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS {
    . = 0x80000000;

    .text : {
        *(.text.start)      /* _start: must be exactly 0x80000000 */
        *(.text*)
    }
    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : { *(.bss*) *(COMMON) }

    /* 64 KiB of stack above everything else */
    . = ALIGN(16);
    . = . + 0x10000;
    __stack_top = .;

    /DISCARD/ : { *(.note*) *(.comment) *(.eh_frame*) }
}
