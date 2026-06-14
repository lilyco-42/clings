CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: lz77

lz77: lz77.c
	$(CC) $(CFLAGS) $< -o $@

test: lz77
	./lz77

clean:
	rm -f lz77

.PHONY: all test clean
