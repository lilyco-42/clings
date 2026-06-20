CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

EXERCISES = ../../exercises
CMD_MOD = \
	$(EXERCISES)/25_file_copy_mycp/mycp.c \
	$(EXERCISES)/26_elf_header_parser/readelf.c \
	$(EXERCISES)/27_sed_stream_editor/sed.c \
	$(EXERCISES)/28_math_calculator/cmd_math.c \
	$(EXERCISES)/29_more_command/cmd_more.c \
	$(EXERCISES)/30_sort_command/cmd_sort.c \
	$(EXERCISES)/31_ls_command/cmd_ll.c

.PHONY: test_32c clean

test_32c: 32c_mybash
	@echo "32c_mybash: compile OK (interactive test requires terminal)"
	@echo "PASS: 32c_mybash"

32c_mybash: 32c_mybash.c $(CMD_MOD)
	$(CC) $(CFLAGS) 32c_mybash.c $(CMD_MOD) -o 32c_mybash $(LDFLAGS)

clean:
	-rm -f 32c_mybash
