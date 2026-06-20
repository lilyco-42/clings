CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 41a_bsearch_iter 41b_bsearch_recur 41c_lower_bound 41d_bsearch_generic

41a_bsearch_iter: 41a_bsearch_iter.c
	$(CC) $(CFLAGS) $< -o $@

41b_bsearch_recur: 41b_bsearch_recur.c
	$(CC) $(CFLAGS) $< -o $@

41c_lower_bound: 41c_lower_bound.c
	$(CC) $(CFLAGS) $< -o $@

41d_bsearch_generic: 41d_bsearch_generic.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 41a_bsearch_iter 41b_bsearch_recur 41c_lower_bound 41d_bsearch_generic
