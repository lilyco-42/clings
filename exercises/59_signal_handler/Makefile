CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: watchdog

watchdog: watchdog.c
	$(CC) $(CFLAGS) $< -o $@

test: watchdog
	./watchdog 2

clean:
	rm -f watchdog

.PHONY: all test clean
