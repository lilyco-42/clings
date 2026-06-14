CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: rle

rle: rle.c
	$(CC) $(CFLAGS) $< -o $@

test: rle
	./rle

clean:
	rm -f rle

.PHONY: all test clean
