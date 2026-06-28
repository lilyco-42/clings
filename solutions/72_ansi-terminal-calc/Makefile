CC      = gcc
CFLAGS  = -Wall -std=c99
TARGET  = terminal_calc
SRC     = terminal_calc.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt && echo "All tests passed."

clean:
	rm -f $(TARGET)

.PHONY: all test clean
