CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = mycp.c
CMD = mycp

.PHONY: all test test_25c clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD)

test: $(CMD)
	echo "hello mycp" > _test_a
	./$(CMD) _test_a _test_b
	diff _test_a _test_b
	@echo "PASS: files are identical"

test_25c: 25c_mycp
	@# Test 1: 普通文本文件拷贝
	echo "hello mycp test" > _test_a
	./25c_mycp _test_a _test_b
	diff _test_a _test_b
	@# Test 2: 空文件拷贝
	printf '' > _test_empty
	./25c_mycp _test_empty _test_empty_cp
	diff _test_empty _test_empty_cp
	@# Test 3: 二进制内容拷贝（含 NULL 字节）
	printf '\x00\x01\x02\xff' > _test_bin
	./25c_mycp _test_bin _test_bin_cp
	diff _test_bin _test_bin_cp
	@# Test 4: 错误路径（源文件不存在应返回非零）
	! ./25c_mycp __nonexistent__ _test_out 2>/dev/null
	@rm -f _test_a _test_b _test_empty _test_empty_cp _test_bin _test_bin_cp _test_out
	@echo "PASS: 25c_mycp"

25c_mycp: 25c_mycp.c
	$(CC) $(CFLAGS) 25c_mycp.c -o 25c_mycp

clean:
	-rm -f _test_a _test_b _test_empty _test_empty_cp _test_bin _test_bin_cp _test_out $(CMD) 25c_mycp
