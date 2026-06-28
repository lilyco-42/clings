CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = lu_solver
SRC = lu_solver.c
LDLIBS = -lm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt
	@echo "PASS: All tests passed."

clean:
	rm -f $(TARGET)

.PHONY: all test clean
