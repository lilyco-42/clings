CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: heap

heap: heap.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f heap

.PHONY: all clean
