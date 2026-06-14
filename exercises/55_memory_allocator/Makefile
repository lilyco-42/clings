CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: mymalloc

mymalloc: mymalloc.c
	$(CC) $(CFLAGS) $< -o $@

test: mymalloc
	./mymalloc

clean:
	rm -f mymalloc

.PHONY: all test clean
