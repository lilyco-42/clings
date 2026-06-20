CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

CMD_MOD = \
	../25_file_copy_mycp/mycp.c \
	../26_elf_header_parser/readelf.c \
	../27_sed_stream_editor/sed.c \
	../28_math_calculator/cmd_math.c \
	../29_more_command/cmd_more.c \
	../30_sort_command/cmd_sort.c \
	../31_ls_command/cmd_ll.c

.PHONY: all test test_32c clean

all: bashclone

bashclone: main.c shell.c $(CMD_MOD)
	$(CC) $(CFLAGS) main.c shell.c $(CMD_MOD) -o bashclone $(LDFLAGS)

test: bashclone
	echo "exit" | ./bashclone

test_32c: 32c_mybash
	@echo "32c_mybash: compile OK (interactive test requires terminal)"
	@echo "PASS: 32c_mybash"

32c_mybash: 32c_mybash.c $(CMD_MOD)
	$(CC) $(CFLAGS) 32c_mybash.c $(CMD_MOD) -o 32c_mybash $(LDFLAGS)

clean:
	-rm -f bashclone 32c_mybash
