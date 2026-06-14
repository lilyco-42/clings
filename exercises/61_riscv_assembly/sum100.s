# sum100.s - compute 1 + 2 + ... + 100
# Compile: riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static sum100.s -o sum100
# Run:     qemu-riscv32 ./sum100; echo $?
# Expect:  178 (5050 & 0xFF, since exit code is 8-bit)

.globl _start
_start:
    li t0, 0         # sum = 0
    li t1, 1         # i = 1
    li t2, 101       # upper bound (exclusive)

loop:
    add t0, t0, t1   # sum += i
    addi t1, t1, 1   # i++
    blt t1, t2, loop # if i < 101 goto loop

    andi a0, t0, 0xFF # exit code is only 8-bit
    li a7, 93        # syscall: exit
    ecall
