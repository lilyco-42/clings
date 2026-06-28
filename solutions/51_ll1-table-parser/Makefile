CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = ll1_parser
SRC = ll1_parser.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
