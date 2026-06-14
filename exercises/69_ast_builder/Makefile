CC = gcc
CFLAGS = -Wall -g

all: ast_builder

ast_builder: ast_builder.c
	$(CC) $(CFLAGS) $< -o $@

test: ast_builder
	./ast_builder test.c

clean:
	rm -f ast_builder

.PHONY: all test clean
