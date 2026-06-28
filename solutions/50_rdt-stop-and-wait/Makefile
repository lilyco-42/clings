CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = rdt_stop_wait
SRC = rdt_stop_wait.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
