# Lesson 66 - Project: Complete toolchain integration
# Assembles RISC-V source, generates ELF, runs on QEMU

RV_ASM   = ../64_assembler/rv_asm
ELF_GEN  = ../65_elf_generator/elf_gen
DISASM   = ../63_disassembler/rv_disasm
QEMU     = qemu-riscv32

# GNU toolchain for comparison
GNU_CC   = riscv64-linux-gnu-gcc
GNU_FLAGS = -march=rv32im -mabi=ilp32 -nostdlib -static

TESTS = return42 sum100

all: tools $(TESTS)

tools:
	$(MAKE) -C ../63_disassembler
	$(MAKE) -C ../64_assembler
	$(MAKE) -C ../65_elf_generator

return42: ../61_riscv_assembly/return42.s
	@echo "=== Building $@ with our toolchain ==="
	$(RV_ASM) $< -o $@.bin
	$(ELF_GEN) $@.bin -o $@
	chmod +x $@

sum100: ../61_riscv_assembly/sum100.s
	@echo "=== Building $@ with our toolchain ==="
	$(RV_ASM) $< -o $@.bin
	$(ELF_GEN) $@.bin -o $@
	chmod +x $@

test: all
	@echo ""
	@echo "====== Running with our toolchain ======"
	@echo "--- return42 ---"
	@$(QEMU) ./return42; echo "exit code: $$?"
	@echo "--- sum100 ---"
	@$(QEMU) ./sum100; echo "exit code: $$?"
	@echo ""
	@echo "====== Running with GNU toolchain (comparison) ======"
	@echo "--- return42 (GNU) ---"
	@$(GNU_CC) $(GNU_FLAGS) ../61_riscv_assembly/return42.s -o return42_gnu
	@$(QEMU) ./return42_gnu; echo "exit code: $$?"
	@echo "--- sum100 (GNU) ---"
	@$(GNU_CC) $(GNU_FLAGS) ../61_riscv_assembly/sum100.s -o sum100_gnu
	@$(QEMU) ./sum100_gnu; echo "exit code: $$?"

disasm: all
	@echo "=== Disassemble our output ==="
	$(DISASM) return42.bin
	@echo ""
	$(DISASM) sum100.bin

clean:
	rm -f *.bin return42 sum100 *_gnu

.PHONY: all tools test disasm clean
