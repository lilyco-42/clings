CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: strip_comments

strip_comments: strip_comments.c
	$(CC) $(CFLAGS) $< -o $@

test: strip_comments
	@echo "=== Test: strip_comments ==="
	@echo '/* block */ int x; // line' | ./strip_comments
	@echo '---'
	@echo 'char *s = "not /* comment */";' | ./strip_comments

clean:
	rm -f strip_comments

.PHONY: all test clean
