CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: dijkstra

dijkstra: dijkstra.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f dijkstra

.PHONY: all clean
