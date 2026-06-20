CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_25c clean

test_25c: 25c_mycp
	echo "hello mycp test" > _test_a
	./25c_mycp _test_a _test_b
	diff _test_a _test_b
	@rm -f _test_a _test_b
	@echo "PASS: 25c_mycp"

25c_mycp: 25c_mycp.c
	$(CC) $(CFLAGS) 25c_mycp.c -o 25c_mycp

clean:
	-rm -f _test_a _test_b 25c_mycp
