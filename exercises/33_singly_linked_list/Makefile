CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all test test_33e clean

all: test_array

test_array: main.c array.c list.c
	$(CC) $(CFLAGS) main.c array.c list.c -o test_array

test: test_array
	./test_array

test_33e: 33e_array_ops
	./33e_array_ops | grep -q "\['a', 'b', 'c', 'd'\]"
	./33e_array_ops | grep -q "length: 4"
	@echo "PASS: 33e_array_ops"

33e_array_ops: 33e_array_ops.c list.c
	$(CC) $(CFLAGS) 33e_array_ops.c list.c -o 33e_array_ops

clean:
	-rm -f test_array 33e_array_ops
