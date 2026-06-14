// runtime_a64.s - AArch64 runtime stubs for nccl-cc (LINUX ONLY)
//
// Provides: _start, putchar, print_int, printf
// Links with compiled output to create standalone AArch64 executables:
//   aarch64-linux-gnu-gcc -nostdlib -static out.s runtime_a64.s
//   qemu-aarch64 ./a.out
//
// Linux AArch64 syscall convention:
//   syscall number in w8, args in x0-x5, 'svc #0' enters the kernel
//   exit = 93, write = 64   (generic numbers, same as riscv64)
//
// AAPCS64 reminders that shape this file:
//   - sp must stay 16-byte aligned at all times (hardware-checked)
//   - x19-x28 are callee-saved, x9-x15 are scratch
//   - x30 (lr) holds the return address; non-leaf functions save it

.text

// _start: entry point, calls main(), then exit(result)
.globl _start
_start:
    bl main
    mov w8, #93          // syscall: exit (status already in x0)
    svc #0

// putchar(int ch): write one character to stdout.
// Leaf function: no lr save needed; buffer lives in a 16-byte slot.
.globl putchar
putchar:
    sub sp, sp, #16
    strb w0, [sp]        // store char on stack
    mov x0, #1           // fd = stdout
    mov x1, sp           // buf = &char
    mov x2, #1           // len = 1
    mov w8, #64          // syscall: write
    svc #0
    add sp, sp, #16
    ret

// print_int(int n): print signed integer to stdout (recursive)
.globl print_int
print_int:
    stp x19, x30, [sp, #-16]!
    mov w19, w0

    // handle negative
    cmp w19, #0
    b.ge .pi_pos
    mov w0, #45          // '-'
    bl putchar
    neg w19, w19

.pi_pos:
    mov w9, #10
    sdiv w10, w19, w9    // w10 = n / 10
    cbz w10, .pi_single

    // recursively print n/10
    mov w0, w10
    bl print_int

.pi_single:
    mov w9, #10
    sdiv w10, w19, w9
    msub w0, w10, w9, w19   // w0 = n % 10
    add w0, w0, #48         // to ASCII
    bl putchar

    ldp x19, x30, [sp], #16
    ret

// printf(fmt, ...): simplified printf supporting %d, %c and %s.
// Compiled code passes at most 8 register args (fmt + 7 values).
// The 7 potential values are spilled to an 8-byte-slot array because
// %s arguments are 64-bit pointers on this target.
.globl printf
printf:
    stp x29, x30, [sp, #-16]!
    stp x19, x20, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    sub sp, sp, #64

    // spill the seven potential value args
    stp x1, x2, [sp, #0]
    stp x3, x4, [sp, #16]
    stp x5, x6, [sp, #32]
    str x7, [sp, #48]

    mov x19, x0          // x19 = fmt pointer
    mov x20, sp          // x20 = arg array base
    mov w21, #0          // w21 = arg index

.pf_loop:
    ldrb w0, [x19]
    cbz w0, .pf_done

    cmp w0, #37          // '%'
    b.ne .pf_char

    // format specifier
    add x19, x19, #1
    ldrb w0, [x19]

    cmp w0, #100         // 'd'
    b.ne .pf_check_c
    ldr x0, [x20, w21, sxtw #3]   // arg[i] (8-byte slots)
    add w21, w21, #1
    bl print_int
    b .pf_next

.pf_check_c:
    cmp w0, #99          // 'c'
    b.ne .pf_check_s
    ldr x0, [x20, w21, sxtw #3]
    add w21, w21, #1
    bl putchar
    b .pf_next

.pf_check_s:
    cmp w0, #115         // 's'
    b.ne .pf_next
    ldr x22, [x20, w21, sxtw #3]  // string pointer (full 64 bits)
    add w21, w21, #1
.pf_str_loop:
    ldrb w0, [x22]
    cbz w0, .pf_next
    bl putchar
    add x22, x22, #1
    b .pf_str_loop

.pf_char:
    // literal character (escape sequences are already decoded
    // by the compiler's tokenizer - bytes here are real bytes)
    bl putchar

.pf_next:
    add x19, x19, #1
    b .pf_loop

.pf_done:
    add sp, sp, #64
    ldp x21, x22, [sp], #16
    ldp x19, x20, [sp], #16
    ldp x29, x30, [sp], #16
    ret

.section .note.GNU-stack,"",@progbits
