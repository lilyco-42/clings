# test.s - simple test for rv_asm assembler
# Expected: assemble and verify with rv_disasm
_start:
    li a0, 42
    li a7, 93
    ecall
