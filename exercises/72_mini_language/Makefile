CC = gcc
CFLAGS = -Wall -g

SRCS = minilang.c
TARGET = minilang

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $^ -o $@

test: $(TARGET)
	@echo "=== fact(5) ==="
	./$(TARGET) test_fact.ml
	@echo "=== fib(10) ==="
	./$(TARGET) test_fib.ml

clean:
	rm -f $(TARGET)

.PHONY: all test clean
