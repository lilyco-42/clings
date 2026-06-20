CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = cmd_ll.c
CMD = ll

.PHONY: all test test_31a test_31b test_31c clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD)

test: $(CMD)
	./$(CMD) Makefile
	./$(CMD) Makefile -i
	./$(CMD) Makefile -s
	./$(CMD) Makefile -T

test_31a: 31a_file_size
	@# Test 1: 已知大小文件
	@printf 'hello' > _test_31a.txt
	./31a_file_size _test_31a.txt | grep -q "size: 5"
	@# Test 2: 空文件
	@printf '' > _test_31a_empty.txt
	./31a_file_size _test_31a_empty.txt | grep -q "size: 0"
	@# Test 3: 错误路径（不存在的文件应返回非零）
	! ./31a_file_size __nonexistent_file__ 2>/dev/null
	@rm -f _test_31a.txt _test_31a_empty.txt
	@echo "PASS: 31a_file_size"

31a_file_size: 31a_file_size.c
	$(CC) $(CFLAGS) 31a_file_size.c -o 31a_file_size

test_31b: 31b_file_time
	@# 验证输出格式为 "mtime: YYYY-MM-DD HH:MM:SS"
	./31b_file_time Makefile | grep -qE "^mtime: [0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$$"
	@echo "PASS: 31b_file_time"

31b_file_time: 31b_file_time.c
	$(CC) $(CFLAGS) 31b_file_time.c -o 31b_file_time

test_31c: 31c_ls_l
	@# 验证不同选项的输出
	./31c_ls_l Makefile | grep -q "Access mode:"
	./31c_ls_l Makefile | grep -q "file uid ="
	./31c_ls_l Makefile | grep -q "file size ="
	./31c_ls_l Makefile -i | grep -qE "file st_ino ="
	./31c_ls_l Makefile -s | grep -qE "file st_blocks ="
	./31c_ls_l Makefile -T | grep -qE "The time is"
	@echo "PASS: 31c_ls_l"

31c_ls_l: 31c_ls_l.c
	$(CC) $(CFLAGS) 31c_ls_l.c -o 31c_ls_l

clean:
	-rm -f $(CMD) 31a_file_size 31b_file_time 31c_ls_l _test_31a.txt _test_31a_empty.txt
