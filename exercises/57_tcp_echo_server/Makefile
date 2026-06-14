CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: echo_server

echo_server: echo_server.c
	$(CC) $(CFLAGS) $< -o $@

test: echo_server
	@echo "Start server: ./echo_server 9000"
	@echo "Test:  echo 'hello' | nc localhost 9000"

clean:
	rm -f echo_server

.PHONY: all test clean
