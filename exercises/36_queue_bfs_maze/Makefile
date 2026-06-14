CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: queue

queue: queue.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f queue

.PHONY: all clean
