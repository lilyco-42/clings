CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all test clean

all: clist_demo

clist_demo: main.c clist.c
	$(CC) $(CFLAGS) main.c clist.c -o clist_demo

test: clist_demo
	echo "10 3" | ./clist_demo

clean:
	-rm -f clist_demo
