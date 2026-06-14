CC = gcc
CFLAGS = -Wall -g

all: elf_gen

elf_gen: elf_gen.c
	$(CC) $(CFLAGS) $< -o $@

test: elf_gen
	@echo "=== Generate ELF from Lesson-64 test.bin ==="
	../Lesson-64/rv_asm ../Lesson-64/test.s -o test.bin
	./elf_gen test.bin -o test_elf
	chmod +x test_elf
	readelf -h test_elf
	@echo "=== Run with QEMU ==="
	qemu-riscv32 ./test_elf; echo "exit: $$?"

clean:
	rm -f elf_gen test.bin test_elf

.PHONY: all test clean
