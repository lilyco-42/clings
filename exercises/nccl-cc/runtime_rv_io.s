# runtime_rv_io.s - platform-INDEPENDENT output helpers for RV32
#
# Provides: print_int, printf (%d %c %s)
#
# This layer is pure logic: it only ever calls putchar(). Pair it with
# a platform layer that provides _start and putchar:
#   runtime_rv.s       Linux user space (write/exit syscalls, qemu-riscv32)
#   runtime_rv_bare.s  bare metal (UART MMIO, qemu-system-riscv32 virt)
#
# Lesson 97: the SAME compiled program + the SAME io layer runs on
# Linux and on bare metal - only putchar/_start/exit differ.

.text

# print_int(int n): print signed integer to stdout (recursive)
.globl print_int
print_int:
    addi sp, sp, -16
    sw ra, 12(sp)
    sw s0, 8(sp)
    sw a0, 4(sp)       # save n

    # handle negative
    bge a0, zero, .pi_pos
    li a0, 45           # '-'
    call putchar
    lw a0, 4(sp)
    neg a0, a0
    sw a0, 4(sp)

.pi_pos:
    lw a0, 4(sp)
    li t0, 10
    div t1, a0, t0     # t1 = n / 10
    beqz t1, .pi_single

    # recursively print n/10
    mv a0, t1
    call print_int

.pi_single:
    lw a0, 4(sp)
    li t0, 10
    rem a0, a0, t0     # a0 = n % 10
    addi a0, a0, 48    # convert to ASCII '0'-'9'
    call putchar

    lw ra, 12(sp)
    lw s0, 8(sp)
    addi sp, sp, 16
    ret

# printf(fmt, ...): simplified printf supporting %d, %c and %s
.globl printf
printf:
    addi sp, sp, -48
    sw ra, 44(sp)
    sw s0, 40(sp)
    sw s1, 36(sp)
    sw s2, 32(sp)
    sw s3, 28(sp)
    # save potential args a1-a5
    sw a1, 0(sp)
    sw a2, 4(sp)
    sw a3, 8(sp)
    sw a4, 12(sp)
    sw a5, 16(sp)

    mv s0, a0          # s0 = fmt string pointer
    addi s1, sp, 0     # s1 = pointer to args on stack
    li s2, 0           # s2 = arg index

.pf_loop:
    lb a0, 0(s0)
    beqz a0, .pf_done

    li t0, 37           # '%'
    bne a0, t0, .pf_char

    # format specifier
    addi s0, s0, 1
    lb a0, 0(s0)

    li t0, 100          # 'd'
    bne a0, t0, .pf_check_c

    # %d: print integer
    slli t0, s2, 2
    add t0, s1, t0
    lw a0, 0(t0)
    addi s2, s2, 1
    call print_int
    j .pf_next

.pf_check_c:
    li t0, 99           # 'c'
    bne a0, t0, .pf_check_s

    # %c: print character
    slli t0, s2, 2
    add t0, s1, t0
    lw a0, 0(t0)
    addi s2, s2, 1
    call putchar
    j .pf_next

.pf_check_s:
    li t0, 115          # 's'
    bne a0, t0, .pf_next

    # %s: print NUL-terminated string
    slli t0, s2, 2
    add t0, s1, t0
    lw s3, 0(t0)
    addi s2, s2, 1
.pf_str_loop:
    lb a0, 0(s3)
    beqz a0, .pf_next
    call putchar
    addi s3, s3, 1
    j .pf_str_loop

.pf_char:
    # literal character (escape sequences are already decoded
    # by the compiler's tokenizer - bytes here are real bytes)
    call putchar

.pf_next:
    addi s0, s0, 1
    j .pf_loop

.pf_done:
    lw ra, 44(sp)
    lw s0, 40(sp)
    lw s1, 36(sp)
    lw s2, 32(sp)
    lw s3, 28(sp)
    addi sp, sp, 48
    ret

.section .note.GNU-stack,"",@progbits
