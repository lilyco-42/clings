# runtime_arm.s - ARM (ARMv7) runtime stubs for nccl-cc (LINUX ONLY)
#
# Provides: _start, putchar, print_int, printf
# Links with compiled output to create standalone ARM executables:
#   arm-linux-gnueabihf-gcc -marm -nostdlib -static out.s runtime_arm.s
#   qemu-arm ./a.out
#
# Linux ARM EABI syscall convention:
#   syscall number in r7, args in r0-r5, 'svc #0' enters the kernel
#   exit = 1, write = 4

.syntax unified
.arch armv7ve
.text

# _start: entry point, calls main(), then exit(result)
.globl _start
_start:
    bl main
    mov r7, #1          @ syscall: exit (status already in r0)
    svc #0

# putchar(int ch): write one character to stdout
.globl putchar
putchar:
    push {r7, lr}
    sub sp, sp, #8
    strb r0, [sp]       @ store char on stack
    mov r0, #1          @ fd = stdout
    mov r1, sp          @ buf = &char
    mov r2, #1          @ len = 1
    mov r7, #4          @ syscall: write
    svc #0
    add sp, sp, #8
    pop {r7, pc}

# print_int(int n): print signed integer to stdout (recursive)
.globl print_int
print_int:
    push {r4, lr}
    mov r4, r0

    @ handle negative
    cmp r4, #0
    bge .pi_pos
    mov r0, #45         @ '-'
    bl putchar
    rsb r4, r4, #0

.pi_pos:
    mov r1, #10
    sdiv r2, r4, r1     @ r2 = n / 10
    cmp r2, #0
    beq .pi_single

    @ recursively print n/10
    mov r0, r2
    bl print_int

.pi_single:
    mov r1, #10
    sdiv r2, r4, r1
    mls r0, r2, r1, r4  @ r0 = n % 10
    add r0, r0, #48     @ to ASCII
    bl putchar

    pop {r4, pc}

# printf(fmt, ...): simplified printf supporting %d, %c and %s.
# Compiled code passes at most 4 register args (fmt + 3 values).
.globl printf
printf:
    push {r4, r5, r6, r8, lr}
    sub sp, sp, #12

    @ spill the three potential value args to a stack array
    str r1, [sp, #0]
    str r2, [sp, #4]
    str r3, [sp, #8]

    mov r4, r0          @ r4 = fmt pointer
    mov r5, sp          @ r5 = arg array base
    mov r6, #0          @ r6 = arg index

.pf_loop:
    ldrb r0, [r4]
    cmp r0, #0
    beq .pf_done

    cmp r0, #37         @ '%'
    bne .pf_char

    @ format specifier
    add r4, r4, #1
    ldrb r0, [r4]

    cmp r0, #100        @ 'd'
    bne .pf_check_c
    ldr r0, [r5, r6, lsl #2]
    add r6, r6, #1
    bl print_int
    b .pf_next

.pf_check_c:
    cmp r0, #99         @ 'c'
    bne .pf_check_s
    ldr r0, [r5, r6, lsl #2]
    add r6, r6, #1
    bl putchar
    b .pf_next

.pf_check_s:
    cmp r0, #115        @ 's'
    bne .pf_next
    ldr r8, [r5, r6, lsl #2]
    add r6, r6, #1
.pf_str_loop:
    ldrb r0, [r8]
    cmp r0, #0
    beq .pf_next
    bl putchar
    add r8, r8, #1
    b .pf_str_loop

.pf_char:
    @ literal character (escape sequences are already decoded
    @ by the compiler's tokenizer - bytes here are real bytes)
    bl putchar

.pf_next:
    add r4, r4, #1
    b .pf_loop

.pf_done:
    add sp, sp, #12
    pop {r4, r5, r6, r8, pc}

.section .note.GNU-stack,"",%progbits
