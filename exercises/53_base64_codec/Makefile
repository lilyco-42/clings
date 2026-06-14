CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: base64

base64: base64.c
	$(CC) $(CFLAGS) $< -o $@

test: base64
	./base64

clean:
	rm -f base64

.PHONY: all test clean
