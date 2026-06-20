CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = readelf.c
CMD = readelf

.PHONY: all test clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD)

test: $(CMD)
	$(CC) $(CFLAGS) -m32 -c main.c -o test_main.o 2>/dev/null || $(CC) $(CFLAGS) -c main.c -o test_main.o
	./$(CMD) test_main.o
	@echo "PASS"

clean:
	-rm -f $(CMD) test_main.o
