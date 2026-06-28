CC      = gcc
CFLAGS  = -Wall -std=c99
TARGET  = terminal_calc
SRC     = terminal_calc.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
