# Makefile for 69_tf-idf-cosine-sim

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = tfidf
SRC     = tfidf.c
LDLIBS  = -lm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt
	@echo "PASS: All tests passed."

clean:
	rm -f $(TARGET)

.PHONY: all test clean
