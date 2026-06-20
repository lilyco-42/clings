CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_33c clean

test_33c: 33c_array_ops
	./33c_array_ops | grep -q "\['a', 'b', 'c', 'd'\]"
	./33c_array_ops | grep -q "length: 4"
	@echo "PASS: 33c_array_ops"

33c_array_ops: 33c_array_ops.c list.c list.h
	$(CC) $(CFLAGS) 33c_array_ops.c list.c -o 33c_array_ops

clean:
	-rm -f 33c_array_ops
