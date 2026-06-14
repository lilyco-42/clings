# runtime_rv.s - LINUX platform layer for RV32
#
# Provides: _start, putchar  (pair with runtime_rv_io.s for printf)
# Target:   Linux user space, run with qemu-riscv32
#
# Lesson 93: Standard library stubs via Linux syscalls.
# Lesson 97 splits the runtime in two layers:
#   runtime_rv_io.s   print_int/printf - pure logic, calls putchar()
#   runtime_rv.s      THIS FILE - how a Linux process starts and prints
#   runtime_rv_bare.s the same contract on bare metal (UART instead of
#                     a write syscall, test finisher instead of exit)

.text

# _start: entry point, calls main(), then exit(result)
.globl _start
_start:
    call main
    mv a0, a0         # return value already in a0
    li a7, 93          # syscall: exit
    ecall

# putchar(int ch): write one character to stdout
.globl putchar
putchar:
    addi sp, sp, -16
    sw a0, 0(sp)       # store character on stack
    li a0, 1           # fd = stdout
    mv a1, sp          # buf = &char on stack
    li a2, 1           # len = 1
    li a7, 64          # syscall: write
    ecall
    addi sp, sp, 16
    ret
