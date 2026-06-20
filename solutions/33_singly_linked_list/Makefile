CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_33e clean

test_33e: 33e_array_ops
	./33e_array_ops | grep -q "\['a', 'b', 'c', 'd'\]"
	./33e_array_ops | grep -q "length: 4"
	@echo "PASS: 33e_array_ops"

33e_array_ops: 33e_array_ops.c list.c list.h
	$(CC) $(CFLAGS) 33e_array_ops.c list.c -o 33e_array_ops

clean:
	-rm -f 33e_array_ops
