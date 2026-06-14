CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: coroutine

coroutine: coroutine.c
	$(CC) $(CFLAGS) $< -o $@

test: coroutine
	./coroutine

clean:
	rm -f coroutine

.PHONY: all test clean
