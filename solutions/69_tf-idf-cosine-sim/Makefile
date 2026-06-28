# Makefile for 69_tf-idf-cosine-sim

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = tfidf
SRC     = tfidf.c
LDLIBS  = -lm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
