CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 45a_lcs_length 45b_lcs_string 45c_diff_simple

45a_lcs_length: 45a_lcs_length.c
	$(CC) $(CFLAGS) $< -o $@

45b_lcs_string: 45b_lcs_string.c
	$(CC) $(CFLAGS) $< -o $@

45c_diff_simple: 45c_diff_simple.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 45a_lcs_length 45b_lcs_string 45c_diff_simple
