CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = pow_chain
SRC     = pow_chain.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: all  clean
