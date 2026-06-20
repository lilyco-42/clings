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
	@echo "--- 25c: text file copy ---"
	echo "hello mycp test" > _test_a
	./25c_mycp _test_a _test_b
	diff _test_a _test_b && echo "  diff OK: text file identical"
	@echo "--- 25c: empty file copy ---"
	printf '' > _test_empty
	./25c_mycp _test_empty _test_empty_cp
	diff _test_empty _test_empty_cp && echo "  diff OK: empty file identical"
	@echo "--- 25c: binary file copy ---"
	printf '\x00\x01\x02\xff' > _test_bin
	./25c_mycp _test_bin _test_bin_cp
	diff _test_bin _test_bin_cp && echo "  diff OK: binary file identical"
	@echo "--- 25c: nonexistent source (expect error) ---"
	! ./25c_mycp __nonexistent__ _test_out 2>/dev/null
	@rm -f _test_a _test_b _test_empty _test_empty_cp _test_bin _test_bin_cp _test_out
	@echo "PASS: 25c_mycp"

25c_mycp: 25c_mycp.c
	$(CC) $(CFLAGS) 25c_mycp.c -o 25c_mycp

clean:
	-rm -f _test_a _test_b _test_empty _test_empty_cp _test_bin _test_bin_cp _test_out $(CMD) 25c_mycp
