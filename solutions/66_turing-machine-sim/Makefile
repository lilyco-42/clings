CC      = gcc
CFLAGS  = -Wall -std=c99
TARGET  = turing_machine
SRC     = turing_machine.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
