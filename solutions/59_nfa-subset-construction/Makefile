CC      = gcc
CFLAGS  = -Wall -std=c99
TARGET  = nfa_dfa
SRC     = nfa_dfa.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

.PHONY: all  clean
