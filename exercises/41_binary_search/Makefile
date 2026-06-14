CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: bsearch

bsearch: bsearch.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f bsearch

.PHONY: all clean
