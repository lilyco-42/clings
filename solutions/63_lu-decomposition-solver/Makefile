CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = lu_solver
SRC = lu_solver.c
LDLIBS = -lm

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $< -o $@ $(LDLIBS)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
