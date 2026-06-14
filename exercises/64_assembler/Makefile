CC = gcc
CFLAGS = -Wall -g

all: rv_asm

rv_asm: rv_asm.c
	$(CC) $(CFLAGS) $< -o $@

test: rv_asm
	@echo "=== Assemble test program ==="
	./rv_asm test.s -o test.bin
	@echo "=== Disassemble with Lesson-63 tool ==="
	../Lesson-63/rv_disasm test.bin

clean:
	rm -f rv_asm *.bin

.PHONY: all test clean
