CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = bplus_tree
SRC     = bplus_tree.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

test: $(TARGET)
	./$(TARGET) | diff - expected_output.txt

clean:
	rm -f $(TARGET)

.PHONY: all test clean
