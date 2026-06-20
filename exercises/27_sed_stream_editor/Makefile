CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = sed.c
CMD = mysed

.PHONY: all test clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD)

test: $(CMD)
	echo "unix is great os. unix is opensource. unix is free os." | ./$(CMD) 's/unix/linux'
	echo "unix is great os. unix is opensource. unix is free os." | ./$(CMD) 's/unix/linux/g'
	echo "unix is great os. unix is opensource. unix is free os." | ./$(CMD) 's/unix/linux/3'
	echo "unix is great os. unix is opensource. unix is free os." | ./$(CMD) 's/^unix/linux'
	@echo "PASS"

clean:
	-rm -f $(CMD)
