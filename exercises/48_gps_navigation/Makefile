CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: gps

gps: gps.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f gps

.PHONY: all clean
