CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: kmp

kmp: kmp.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f kmp

.PHONY: all clean
