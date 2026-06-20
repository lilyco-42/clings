CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = cmd_sort.c
CMD = sort

.PHONY: all test clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD)

test: $(CMD)
	./$(CMD) test.txt
	./$(CMD) test.txt -u
	./$(CMD) test.txt -r
	./$(CMD) test.txt -n

clean:
	-rm -f $(CMD)
