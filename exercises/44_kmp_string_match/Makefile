CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 44a_naive_search 44b_build_next 44c_kmp_search

44a_naive_search: 44a_naive_search.c
	$(CC) $(CFLAGS) $< -o $@

44b_build_next: 44b_build_next.c
	$(CC) $(CFLAGS) $< -o $@

44c_kmp_search: 44c_kmp_search.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 44a_naive_search 44b_build_next 44c_kmp_search
