CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 40a_dict_load 40b_dict_lookup 40c_dict_stats

40a_dict_load: 40a_dict_load.c
	$(CC) $(CFLAGS) $< -o $@

40b_dict_lookup: 40b_dict_lookup.c
	$(CC) $(CFLAGS) $< -o $@

40c_dict_stats: 40c_dict_stats.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 40a_dict_load 40b_dict_lookup 40c_dict_stats
