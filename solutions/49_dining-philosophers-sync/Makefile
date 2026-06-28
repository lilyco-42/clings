CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = dining_philosophers
SRC = dining_philosophers.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
