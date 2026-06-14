CC = gcc
CFLAGS = -Wall -g

all: stack_vm

stack_vm: stack_vm.c
	$(CC) $(CFLAGS) $< -o $@

test: stack_vm
	./stack_vm

clean:
	rm -f stack_vm

.PHONY: all test clean
