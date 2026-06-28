# Makefile for 65_mark-sweep-gc (solution)
CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -O2
TARGET  = mark_sweep
SRC     = mark_sweep.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lm

test: $(TARGET)
	./$(TARGET)
	@echo ""
	@echo "=== Diff against expected ==="
	./$(TARGET) | diff - expected_output.txt && echo "OK: output matches" || echo "FAIL: output differs"

clean:
	rm -f $(TARGET)

.PHONY: all test clean
