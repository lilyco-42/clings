CC      = gcc
CFLAGS  = -Wall -std=c99
LDFLAGS = -lm
TARGET  = qubit
SRC     = qubit.c

all: $(TARGET)

$(TARGET): $(SRC) complex.h
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt && echo "All tests passed."

clean:
	rm -f $(TARGET)

.PHONY: all test clean
