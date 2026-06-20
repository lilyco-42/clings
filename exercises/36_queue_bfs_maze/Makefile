CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: queue_demo

queue_demo: queue.c
	$(CC) $(CFLAGS) queue.c -o queue_demo

clean:
	-rm -f queue_demo
