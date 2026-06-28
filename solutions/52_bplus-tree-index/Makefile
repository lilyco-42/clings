CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = bplus_tree
SRC     = bplus_tree.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
