CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = mycp.c
CMD = mycp

.PHONY: all test clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD)

test: $(CMD)
	echo "hello mycp" > a
	./$(CMD) a b
	diff a b
	@echo "PASS: files are identical"

clean:
	-rm -f a b $(CMD)
