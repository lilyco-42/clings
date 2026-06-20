CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 42a_partition 42b_quicksort 42c_qsort_generic

42a_partition: 42a_partition.c
	$(CC) $(CFLAGS) $< -o $@

42b_quicksort: 42b_quicksort.c
	$(CC) $(CFLAGS) $< -o $@

42c_qsort_generic: 42c_qsort_generic.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 42a_partition 42b_quicksort 42c_qsort_generic
