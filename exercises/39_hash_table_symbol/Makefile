CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 39a_hash_func 39b_hash_insert 39c_hash_delete 39d_symbol_table

39a_hash_func: 39a_hash_func.c
	$(CC) $(CFLAGS) $< -o $@

39b_hash_insert: 39b_hash_insert.c
	$(CC) $(CFLAGS) $< -o $@

39c_hash_delete: 39c_hash_delete.c
	$(CC) $(CFLAGS) $< -o $@

39d_symbol_table: 39d_symbol_table.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 39a_hash_func 39b_hash_insert 39c_hash_delete 39d_symbol_table
