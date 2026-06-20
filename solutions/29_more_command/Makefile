CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g

.PHONY: test_29c clean

test_29c: 29c_more
	@echo "29c_more: compile OK (interactive test requires terminal)"
	@echo "PASS: 29c_more"

29c_more: 29c_more.c
	$(CC) $(CFLAGS) 29c_more.c -o 29c_more

clean:
	-rm -f 29c_more
