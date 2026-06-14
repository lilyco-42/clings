CC = gcc
CFLAGS = -Wall -g

all: rv_disasm

rv_disasm: rv_disasm.c
	$(CC) $(CFLAGS) $< -o $@

test: rv_disasm
	@echo "=== Disassemble Lesson-61 fact.s binary ==="
	riscv64-linux-gnu-gcc -march=rv32im -mabi=ilp32 -nostdlib -static ../Lesson-61/fact.s -o /tmp/fact_rv
	riscv64-linux-gnu-objcopy -O binary -j .text /tmp/fact_rv /tmp/fact_rv.bin
	./rv_disasm /tmp/fact_rv.bin

clean:
	rm -f rv_disasm

.PHONY: all test clean
