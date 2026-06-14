CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: qsort

qsort: qsort.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f qsort

.PHONY: all clean
