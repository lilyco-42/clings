CC = gcc
CFLAGS = -Wall -Wextra -std=c11

all: http_server

http_server: http_server.c
	$(CC) $(CFLAGS) $< -o $@

test: http_server
	@echo "Start: ./http_server 8080 ."
	@echo "Test:  curl http://localhost:8080/index.html"

clean:
	rm -f http_server

.PHONY: all test clean
