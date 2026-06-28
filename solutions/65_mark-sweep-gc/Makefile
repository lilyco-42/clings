# Makefile for 65_mark-sweep-gc (solution)
CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -O2
TARGET  = mark_sweep
SRC     = mark_sweep.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) -lm

clean:
	rm -f $(TARGET)

.PHONY: all  clean
