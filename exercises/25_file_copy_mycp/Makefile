CC ?= gcc
CFLAGS = -Wall -Wextra -std=c11 -g
SRC = mycp.c
CMD = mycp

.PHONY: all test test_25c clean

all: $(CMD)

$(CMD): main.c $(SRC)
	$(CC) $(CFLAGS) main.c $(SRC) -o $(CMD)

test: $(CMD)
	echo "hello mycp" > _test_a
	./$(CMD) _test_a _test_b
	diff _test_a _test_b
	@echo "PASS: files are identical"

test_25c: 25c_mycp
	echo "hello mycp test" > _test_a
	./25c_mycp _test_a _test_b
	diff _test_a _test_b
	@rm -f _test_a _test_b
	@echo "PASS: 25c_mycp"

25c_mycp: 25c_mycp.c
	$(CC) $(CFLAGS) 25c_mycp.c -o 25c_mycp

clean:
	-rm -f _test_a _test_b $(CMD) 25c_mycp
