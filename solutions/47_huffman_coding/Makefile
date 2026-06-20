CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 47a_freq_count 47b_build_tree 47c_encode 47d_compress

47a_freq_count: 47a_freq_count.c
	$(CC) $(CFLAGS) $< -o $@

47b_build_tree: 47b_build_tree.c
	$(CC) $(CFLAGS) $< -o $@

47c_encode: 47c_encode.c
	$(CC) $(CFLAGS) $< -o $@

47d_compress: 47d_compress.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 47a_freq_count 47b_build_tree 47c_encode 47d_compress
