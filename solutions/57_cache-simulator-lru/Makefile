CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = cache_sim
SRC = cache_sim.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
