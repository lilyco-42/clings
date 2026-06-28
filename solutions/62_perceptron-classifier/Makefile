CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = perceptron
SRC = perceptron.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
