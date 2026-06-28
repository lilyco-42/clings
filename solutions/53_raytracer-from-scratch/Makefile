CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = raytracer
SRC = raytracer.c
LDLIBS = -lm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
