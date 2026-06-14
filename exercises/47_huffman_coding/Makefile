CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: huffman

huffman: huffman.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f huffman

.PHONY: all clean
