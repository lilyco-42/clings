# fact.s - compute factorial(5) = 120 using recursion
# Demonstrates: function call convention, stack frame, ra save/restore
# Compile: riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static fact.s -o fact
# Run:     qemu-riscv32 ./fact; echo $?
# Expect:  120

.globl _start
_start:
    li a0, 5         # n = 5
    call fact        # result = fact(5)
    # a0 now holds 120
    li a7, 93        # syscall: exit
    ecall

# int fact(int n)
# if (n <= 1) return 1;
# return n * fact(n - 1);
fact:
    addi sp, sp, -16 # allocate stack frame
    sw ra, 12(sp)    # save return address
    sw a0, 8(sp)     # save argument n

    li t0, 1
    ble a0, t0, .base # if n <= 1 goto base case

    addi a0, a0, -1  # a0 = n - 1
    call fact        # a0 = fact(n - 1)

    lw t0, 8(sp)     # restore n
    mul a0, t0, a0   # a0 = n * fact(n - 1)
    j .done

.base:
    li a0, 1         # return 1

.done:
    lw ra, 12(sp)    # restore return address
    addi sp, sp, 16  # free stack frame
    ret              # return to caller
