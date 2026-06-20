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
	@# Test 1: exit 命令正常退出
	echo "exit" | ./32c_mybash > /dev/null
	@# Test 2: sort 命令功能验证
	printf 'sort ../../exercises/30_sort_command/test.txt\nexit\n' | ./32c_mybash | grep -q "black"
	@# Test 3: 未知命令报错
	printf 'nonexistent_cmd\nexit\n' | ./32c_mybash 2>&1 | grep -q "command not found"
	@# Test 4: Ctrl+D (EOF) 正常退出
	printf '' | ./32c_mybash > /dev/null
	@echo "PASS: 32c_mybash"

32c_mybash: 32c_mybash.c $(CMD_MOD)
	$(CC) $(CFLAGS) 32c_mybash.c $(CMD_MOD) -o 32c_mybash $(LDFLAGS)

clean:
	-rm -f bashclone 32c_mybash
