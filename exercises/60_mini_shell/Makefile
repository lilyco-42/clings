CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: minishell2

minishell2: minishell2.c
	$(CC) $(CFLAGS) $< -o $@

test: minishell2
	@echo "=== Mini Shell 2.0 Test ==="
	@echo 'echo hello | tr a-z A-Z' | ./minishell2
	@echo 'ls -la | head -5' | ./minishell2

clean:
	rm -f minishell2

.PHONY: all test clean
