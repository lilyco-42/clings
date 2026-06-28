CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = ringbuffer
SRC = ringbuffer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
