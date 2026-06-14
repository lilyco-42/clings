# runtime_rv_bare.s - BARE METAL platform layer for RV32 (no OS at all)
#
# Provides: _start, putchar  (pair with runtime_rv_io.s for printf)
# Target:   qemu-system-riscv32 -machine virt -nographic -bios none
#
# Lesson 97: what does it take to run C with NO operating system?
#   - someone must set up the stack pointer (here: _start, address
#     from the linker script symbol __stack_top)
#   - "output" is not a syscall but a WRITE TO A DEVICE REGISTER:
#     the virt board maps a 16550A UART at physical 0x10000000
#   - "exit" does not exist; qemu's SiFive test finisher device at
#     0x100000 lets a guest ask the EMULATOR to quit with a status
#
# Memory map used here (qemu -machine virt):
#   0x00100000  SiFive test finisher (qemu exits on magic writes)
#   0x10000000  UART0 (16550A: THR at +0, LSR at +5, LSR bit5 = THR empty)
#   0x80000000  RAM base - the mask ROM jumps here, so _start MUST be
#               the very first byte of .text (see bare_rv.ld, which
#               also discards .note sections that would steal the slot)

.section .text.start

# _start: set up sp, run main, report its exit code to the finisher
.globl _start
_start:
    la sp, __stack_top
    call main

    # exit(a0) via the test finisher:
    #   status == 0: write PASS (0x5555)        -> qemu exits 0
    #   status != 0: write FAIL | status << 16  -> qemu exits with status
    li t0, 0x100000
    beqz a0, .exit_pass
    slli t1, a0, 16
    li t2, 0x3333          # FINISHER_FAIL (> 12-bit imm, so via t2)
    or t1, t1, t2
    sw t1, 0(t0)
1:  j 1b                   # qemu handles the shutdown request
                           # asynchronously - spin, NEVER fall through
                           # (a PASS write would zero the exit code)
.exit_pass:
    li t1, 0x5555          # FINISHER_PASS
    sw t1, 0(t0)
2:  j 2b

.text

# putchar(int ch): poll the UART until the transmitter is idle, then
# write the byte to the Transmit Holding Register
.globl putchar
putchar:
    li t0, 0x10000000
.pc_wait:
    lbu t1, 5(t0)          # LSR (Line Status Register)
    andi t1, t1, 0x20      # bit 5: THR empty?
    beqz t1, .pc_wait
    sb a0, 0(t0)           # THR (Transmit Holding Register)
    ret
