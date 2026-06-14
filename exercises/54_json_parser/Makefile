CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: json_parser

json_parser: json_parser.c
	$(CC) $(CFLAGS) $< -o $@

test: json_parser
	./json_parser

clean:
	rm -f json_parser

.PHONY: all test clean
