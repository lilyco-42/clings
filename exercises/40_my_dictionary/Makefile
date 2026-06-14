CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: dict

dict: dict.c
	$(CC) $(CFLAGS) $< -o $@

test: dict
	./dict

clean:
	rm -f dict

.PHONY: all test clean
