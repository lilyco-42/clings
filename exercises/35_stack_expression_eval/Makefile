CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: stack_demo

stack_demo: stack.c
	$(CC) $(CFLAGS) stack.c -o stack_demo

clean:
	-rm -f stack_demo
