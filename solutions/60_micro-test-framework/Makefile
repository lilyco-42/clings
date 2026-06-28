CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = test_framework
SRC = test_framework.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
