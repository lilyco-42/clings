CC = gcc
CFLAGS = -Wall -g

all: interpreter

interpreter: interpreter.c
	$(CC) $(CFLAGS) $< -o $@

test: interpreter
	./interpreter test.c

clean:
	rm -f interpreter

.PHONY: all test clean
