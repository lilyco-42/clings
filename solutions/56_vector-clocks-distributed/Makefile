CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = vector_clocks
SRC = vector_clocks.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
