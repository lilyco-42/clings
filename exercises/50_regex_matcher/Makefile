CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: regex

regex: regex.c
	$(CC) $(CFLAGS) $< -o $@

test: regex
	./regex

clean:
	rm -f regex

.PHONY: all test clean
