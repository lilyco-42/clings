CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = overflow_lab
SRC = overflow_lab.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
