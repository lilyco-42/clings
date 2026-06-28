CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = rsa_crypto
SRC = rsa_crypto.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all  clean
