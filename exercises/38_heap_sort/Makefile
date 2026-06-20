CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 38a_max_heapify 38b_build_heap 38c_heap_sort

38a_max_heapify: 38a_max_heapify.c
	$(CC) $(CFLAGS) $< -o $@

38b_build_heap: 38b_build_heap.c
	$(CC) $(CFLAGS) $< -o $@

38c_heap_sort: 38c_heap_sort.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 38a_max_heapify 38b_build_heap 38c_heap_sort
