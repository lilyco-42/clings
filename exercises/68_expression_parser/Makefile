CC = gcc
CFLAGS = -Wall -g

all: expr_parser

expr_parser: expr_parser.c
	$(CC) $(CFLAGS) $< -o $@

test: expr_parser
	echo "2 + 3 * 4" | ./expr_parser
	echo "(1 + 2) * (3 + 4)" | ./expr_parser

clean:
	rm -f expr_parser

.PHONY: all test clean
