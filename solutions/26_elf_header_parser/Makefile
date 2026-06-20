CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_26a test_26b test_26c clean

_test_elf32.o:
	@echo 'int _test_var;' > _test_elf32.c
	$(CC) -m32 -c _test_elf32.c -o _test_elf32.o
	@rm -f _test_elf32.c

test_26a: 26a_read_magic _test_elf32.o
	@echo "--- 26a: read ELF magic ---"
	./26a_read_magic _test_elf32.o
	./26a_read_magic _test_elf32.o | grep -q "ELF Magic:"
	./26a_read_magic _test_elf32.o | grep -q "7f 45 4c 46"
	@echo "--- 26a: non-ELF file ---"
	./26a_read_magic Makefile 2>/dev/null || true
	./26a_read_magic Makefile 2>/dev/null | grep -q "Not an ELF file"
	@echo "PASS: 26a_read_magic"

26a_read_magic: 26a_read_magic.c
	$(CC) $(CFLAGS) 26a_read_magic.c -o 26a_read_magic

test_26b: 26b_parse_header _test_elf32.o
	@echo "--- 26b: parse ELF header ---"
	./26b_parse_header _test_elf32.o
	./26b_parse_header _test_elf32.o | grep -q "Type:"
	./26b_parse_header _test_elf32.o | grep -q "Machine:"
	@echo "--- 26b: non-ELF file (expect error) ---"
	! ./26b_parse_header Makefile 2>/dev/null
	@echo "PASS: 26b_parse_header"

26b_parse_header: 26b_parse_header.c
	$(CC) $(CFLAGS) 26b_parse_header.c -o 26b_parse_header

test_26c: 26c_file_type _test_elf32.o
	@echo "--- 26c: ELF file type ---"
	./26c_file_type _test_elf32.o
	./26c_file_type _test_elf32.o | grep -q "ELF Header:"
	./26c_file_type _test_elf32.o | grep -q "REL (Relocatable file)"
	./26c_file_type _test_elf32.o | grep -q "Type:"
	@echo "PASS: 26c_file_type"

26c_file_type: 26c_file_type.c
	$(CC) $(CFLAGS) 26c_file_type.c -o 26c_file_type

clean:
	-rm -f _test_elf32.o _test_elf32.c 26a_read_magic 26b_parse_header 26c_file_type
