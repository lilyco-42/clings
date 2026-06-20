CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: all clean

all: 37a_bst_insert 37b_bst_inorder 37c_char_freq 37d_word_freq 37e_bst_delete

37a_bst_insert: 37a_bst_insert.c
	$(CC) $(CFLAGS) $< -o $@

37b_bst_inorder: 37b_bst_inorder.c
	$(CC) $(CFLAGS) $< -o $@

37c_char_freq: 37c_char_freq.c
	$(CC) $(CFLAGS) $< -o $@

37d_word_freq: 37d_word_freq.c
	$(CC) $(CFLAGS) $< -o $@

37e_bst_delete: 37e_bst_delete.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f 37a_bst_insert 37b_bst_inorder 37c_char_freq 37d_word_freq 37e_bst_delete
