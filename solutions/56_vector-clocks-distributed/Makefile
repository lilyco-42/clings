CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = vector_clocks
SRC = vector_clocks.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt
	@echo "PASS: All tests passed."

clean:
	rm -f $(TARGET)

.PHONY: all test clean
