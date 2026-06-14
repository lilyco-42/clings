CC = gcc
CFLAGS = -Wall -g

all: tokenizer

tokenizer: tokenizer.c
	$(CC) $(CFLAGS) $< -o $@

test: tokenizer
	./tokenizer test.c

clean:
	rm -f tokenizer

.PHONY: all test clean
