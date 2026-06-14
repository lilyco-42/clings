# return42.s - the simplest RISC-V program
# Compile: riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static return42.s -o return42
# Run:     qemu-riscv32 ./return42; echo $?
# Expect:  42

.globl _start
_start:
    li a0, 42        # exit code = 42
    li a7, 93        # syscall: exit
    ecall
